#define RAY_MAX_STEPS 1000
#define RAY_MAX_LEN 15.
#define RAY_MIN_HIT_DIST 1e-5

//#define light_dir vec3(-0.36, 0.8, 0.48)


#define SHADOW_HARDNESS 10
#define SHADOW_DARKNESS 0.99

#define BACKGROUND_COLOR vec3(0.51, 0.67, 1.0)
#define LIGHT_COLOR vec3(1.0,0.95,0.8)

#define FRACTAL_ITER 16

uniform vec2 resolution;

uniform vec3 cam_pos;
uniform vec3 cam_forward;
uniform vec3 cam_up;
uniform vec3 cam_right;
uniform float cam_focal_length;

uniform vec3 light_dir = vec3(-0.36, 0.8, 0.48);


uniform float frac_ang1 = -1.606;
uniform float frac_ang2 = -1.275;
uniform float frac_scale = 1.924;
uniform vec3 frac_shift = vec3(-4.15,-5.25, 2.049);
uniform vec3 frac_color = vec3(-0.34,0.12,-0.08);


/*
uniform float frac_ang1 = -9.83;
uniform float frac_ang2 = -1.16;
uniform float frac_scale = 1.9073;
uniform vec3 frac_shift = vec3(-3.508,-3.593,3.295);
uniform vec3 frac_color = vec3(-0.34,0.12,-0.08);
*/

/*
uniform float frac_ang1 = -1.543;
uniform float frac_ang2 = -1.165;
uniform float frac_scale = 2.1;
uniform vec3 frac_shift = vec3(-3.508, -5.95, 2.099);
uniform vec3 frac_color = vec3(-0.34f, 0.12f, -0.08f);
*/


// DEs
float DFSphere(vec4 p, float r)
{
	return (length(p.xyz) - r) / p.w;
}

// folding
void MengerFold(inout vec4 z)
{
	float a = min(z.x - z.y, 0.0);
	z.x -= a;
	z.y += a;
	a = min(z.x - z.z, 0.0);
	z.x -= a;
	z.z += a;
	a = min(z.y - z.z, 0.0);
	z.y -= a;
	z.z += a;
}

//void BoxFold(inout vec4 z, vec3 r)
//{
//	z.xyz = clamp(z.xyz, -r, r) * 2.0 - z.xyz;
//}
void RotX(inout vec4 z, float s, float c) 
{
	z.yz = vec2(c*z.y + s*z.z, c*z.z - s*z.y);
}
//void RotY(inout vec4 z, float s, float c) 
//{
//	z.xz = vec2(c*z.x - s*z.z, c*z.z + s*z.x);
//}
void RotZ(inout vec4 z, float s, float c) 
{
	z.xy = vec2(c*z.x + s*z.y, c*z.y - s*z.x);
}
void RotX(inout vec4 z, float a) 
{
	RotX(z, sin(a), cos(a));
}
//void RotY(inout vec4 z, float a) 
//{
//	RotY(z, sin(a), cos(a));
//}
void RotZ(inout vec4 z, float a) 
{
	RotZ(z, sin(a), cos(a));
}


// DEs
float DESphere(vec4 p, float r)
{
	return (length(p.xyz) - r) / p.w;
}

float DEbox(vec4 pos, vec3 scale) 
{
	vec3 a = abs(pos) - scale;
	return min(max(max(a.x, a.y), a.z), 0.0) + length(max(a, 0.0)) / pos.w;
}

float DEFractal(vec4 pos)
{
	for (int i = 0; i < FRACTAL_ITER; i++)
	{
		pos.xyz = abs(pos.xyz);
		RotZ(pos, frac_ang1);
		MengerFold(pos);
		RotX(pos, frac_ang2);
		pos *= frac_scale;
		pos.xyz += frac_shift;
	}

	return DEbox(pos, vec3(6.0));
}

vec4 ColorFractal(vec4 pos)
{
	vec3 orbit = vec3(0.);
	for (int i = 0; i < FRACTAL_ITER; i++)
	{
		pos.xyz = abs(pos.xyz);
		RotZ(pos, frac_ang1);
		MengerFold(pos);
		RotX(pos, frac_ang2);
		pos *= frac_scale;
		pos.xyz += frac_shift;
		orbit = max(orbit, pos.xyz * frac_color);
	}

	return vec4(orbit, DEbox(pos, vec3(6.0)));
}

float DFScene(vec3 pos)
{
	//return min(DEFractal(vec4(pos, 1)), 
	//min(DFSphere(vec4(pos, 1), 1.),
	//DFSphere(vec4(pos + vec3(2.79286, -5.51317, 0.131162), 1), 0.001))
	//);

	//return min(DEFractal(vec4(pos, 1)),
	//pos.y - -5
	//);

	return DEFractal(vec4(pos, 1));
}

// faster normal function
vec3 GetNormal( vec3 p ) // for function f(p)
{
    const vec2 k = vec2(1, -1);
    return normalize( k.xyy * DFScene( p + k.xyy * RAY_MIN_HIT_DIST ) + 
                      k.yyx * DFScene( p + k.yyx * RAY_MIN_HIT_DIST ) + 
                      k.yxy * DFScene( p + k.yxy * RAY_MIN_HIT_DIST ) + 
                      k.xxx * DFScene( p + k.xxx * RAY_MIN_HIT_DIST ) );
}

// cast a ray and return its length
void CastRay(vec3 ro, vec3 rd, inout float len, inout float min_d, inout float steps)
{
	len = 0.;
	min_d = 1.;

	for (steps = 0; steps < RAY_MAX_STEPS; steps++)
	{
		float df_dist = DFScene(ro + rd * len);
		float surf_dist = max((1. / resolution.x) * len, RAY_MIN_HIT_DIST);

		if (len > RAY_MAX_LEN) break;
		if (df_dist < surf_dist)
		{
			steps += df_dist / surf_dist;
			break;
		}

		len += df_dist;
		min_d = min(min_d, df_dist);
	}
}

float GetShadow(vec3 ro, vec3 rd)//, float min_hit_dist, float max_len)
{
	float k = 1.;
	float min_hit_dist = 2e-5;
	float len = min_hit_dist;

	for (int i = 0; len < RAY_MAX_LEN; i++)
	{
		min_hit_dist += 0.1 * min_hit_dist;
		float df_dist = DFScene(ro + rd * len);

		if (df_dist < min_hit_dist) return 0.;

		k = min(k, SHADOW_HARDNESS * df_dist / len);
		len += df_dist;
	}

	return k;
}

// return color of a ray
vec4 Render(vec3 ro, vec3 rd)
{
	vec3 color;
	float expos = 1;

	vec3 sky_color = BACKGROUND_COLOR - max(rd.y, 0.) * 0.2; // sky
	color = sky_color;

	float sundp = dot(rd, light_dir);
	if (sundp > 0.99) // sun
	{ 
		color += vec3(min(exp((sundp - 1.0 + 0.0001) * 4 / 0.001), 1.0));
	}

	// castray outputs
	float len;
	float min_d;
	float steps;

	// primary ray to surface
	CastRay(ro, rd, len, min_d, steps);

	if (len < RAY_MAX_LEN) // ray hit surface
	{
		vec3 hit_pos = ro + rd * len;

		// get normal of surface
		vec3 norm = GetNormal(hit_pos);

		// raise the hitpos slightly off the surface so it isn't inside it
		hit_pos += norm * RAY_MIN_HIT_DIST;
			
		color = vec3(ColorFractal(vec4(hit_pos, 1) ).xyz);

		// calc light exposure
		expos = GetShadow(hit_pos, light_dir); // shadows
		expos = max(expos, 1. - SHADOW_DARKNESS);

		if (expos > 1. - SHADOW_DARKNESS)
		{
			expos = min(expos, SHADOW_DARKNESS * 0.5 * (dot(norm, light_dir) - 1.) + 1.); // diffuse lighting
			color *= LIGHT_COLOR;
		}

		//k *= GetAO(hit_pos, norm); // HQ AO
		expos *= 1. / (1. + steps * 0.016); // fast free AO

		color = clamp(color, 0, 1);
		color *= expos;
		color = pow(color, vec3(0.4545)); // gamma correction
		color = mix(color, sky_color, len / RAY_MAX_LEN);
		
	}

	//return vec4(color + vec3(len / (RAY_MAX_LEN) * 0.1), pow(expos, 0.4545));
	//color = pow(
	//return vec4(color * expos, 1);
	color = clamp(color, 0, 1);

	return vec4(color, 1);
}

void main()
{
	vec3 ray_origin = cam_pos;
	vec3 ray_direction = normalize(cam_forward * cam_focal_length 
		+ cam_right * (gl_FragCoord.x - resolution.x * 0.5)
		+ cam_up * (gl_FragCoord.y - resolution.y * 0.5));

	gl_FragColor = vec4(Render(ray_origin, ray_direction));
}
