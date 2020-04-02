#include "myvec.h"
#include <math.h>

namespace myvec
{
	Vec3d::Vec3d()
	{
		x = y = z = 0;
	}

	Vec3d::Vec3d(double a)
	{
		x = y = z = a;
	}

	Vec3d::Vec3d(double a, double b, double c)
	{
		x = a;
		y = b;
		z = c;
	}


	Vec3d::~Vec3d()
	{
	}

	double Vec3d::dot(Vec3d v)
	{
		return x * v.x + y * v.y + z * v.z;
	}

	Vec3d Vec3d::cross(Vec3d v)
	{
		return Vec3d(
				y * v.z - z * v.y,
				z * v.x - x * v.z,
				x * v.y - y * v.x
		);
	}

	// https://apps.dtic.mil/dtic/tr/fulltext/u2/1023343.pdf
	Vec3d Vec3d::rotate(Vec3d axis, float theta)
	{
		return *this + axis.cross(*this * sin(theta)) + axis.cross((axis.cross(*this))) * (1 - cos(theta));
	}

	Vec3d Vec3d::normalized()
	{
		double r = sqrt(x * x + y * y + z * z);
		return r > 0 ? Vec3d(x / r, y / r, z / r) : Vec3d();
	}

	Vec3d Vec3d::abs()
	{
		return Vec3d(fabs(x), fabs(y), fabs(z));
	}

	double Vec3d::length2()
	{
		return x * x + y * y + z * z;
	}

	double Vec3d::length()
	{
			return sqrt(length2());
	}

	Vec3d Vec3d::operator -(Vec3d v)
	{
		return Vec3d(x - v.x, y - v.y, z - v.z);
	}

	Vec3d Vec3d::operator +(Vec3d v)
	{
		return Vec3d(x + v.x, y + v.y, z + v.z);
	}

	Vec3d Vec3d::operator *(double n)
	{
		return Vec3d(x *n, y * n, z * n);
	}

	Vec3d Vec3d::operator *(Vec3d v)
	{
		return Vec3d(x * v.x, y * v.y, z * v.z);
	}

	void Vec3d::operator +=(Vec3d v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
	}
}