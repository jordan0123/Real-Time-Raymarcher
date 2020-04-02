#include "scene.h"
#include <math.h>
#include <algorithm>

# define M_PI 3.14159265358979323846

Scene::Scene(sf::Shader* s)
{
	shader = s;

	cam_pos = myvec::Vec3d(-2.79286, 5.51317, -0.131162);

	cam_forward	= myvec::Vec3d(0, 0, 1);
	cam_up		= myvec::Vec3d(0, 1, 0);
	cam_right	= myvec::Vec3d(1, 0, 0);

	cam_focal_length = 700;
	//max_ray_steps = 100;
	//min_ray_dist = 0.00001f;

	scale = 1;

	shader->setUniform("cam_pos", sf::Glsl::Vec3(cam_pos.x, cam_pos.y, cam_pos.z));
	shader->setUniform("cam_forward", sf::Glsl::Vec3(cam_forward.x, cam_forward.y, cam_forward.z));
	shader->setUniform("cam_up", sf::Glsl::Vec3(cam_up.x, cam_up.y, cam_up.z));
	shader->setUniform("cam_right", sf::Glsl::Vec3(cam_right.x, cam_right.y, cam_right.z));
	shader->setUniform("cam_focal_length", cam_focal_length);
	//shader->setUniform("max_ray_steps", max_ray_steps);
	//shader->setUniform("min_ray_dist", min_ray_dist);

}

Scene::~Scene()
{
}

void Scene::TurnCam(myvec::Vec3d ang)
{
	// pitch
	cam_forward = cam_forward.rotate(cam_right, ang.x);
	cam_up = cam_up.rotate(cam_right, ang.x);
	
	// yaw
	cam_right = cam_right.rotate(cam_up, ang.y);
	cam_forward = cam_forward.rotate(cam_up, ang.y);

	// roll
	cam_right = cam_right.rotate(cam_forward, ang.z);
	cam_up = cam_up.rotate(cam_forward, ang.z);

}

void Scene::SetCamAng(myvec::Vec3d ang)
{
	//cam_ang = ang;
}

void Scene::SetCamPos(myvec::Vec3d pos)
{
	cam_pos = pos;
}

void Scene::MoveCam(myvec::Vec3d v)
{
	cam_pos +=
			cam_right * (v.x / scale)
			+ cam_up * (v.y / scale)
			+ cam_forward * (v.z / scale);
}

void Scene::UpdateCam()
{

	shader->setUniform("cam_pos", sf::Glsl::Vec3(cam_pos.x, cam_pos.y, cam_pos.z));
	shader->setUniform("cam_focal_length", cam_focal_length);

	shader->setUniform("cam_forward", sf::Glsl::Vec3(cam_forward.x, cam_forward.y, cam_forward.z));
	shader->setUniform("cam_up", sf::Glsl::Vec3(cam_up.x, cam_up.y, cam_up.z));
	shader->setUniform("cam_right", sf::Glsl::Vec3(cam_right.x, cam_right.y, cam_right.z));
}

