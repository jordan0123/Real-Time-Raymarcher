#ifndef MYVEC_H
#define MYVEC_H

namespace myvec
{
	class Vec3d
	{
	public:
		double x;
		double y;
		double z;

		Vec3d();
		Vec3d(double a);
		Vec3d(double a, double b, double c);

		double dot(Vec3d v);
		Vec3d cross(Vec3d v);
		Vec3d normalized();
		Vec3d abs();
		double length();
		double length2();
		Vec3d rotate(Vec3d axis, float theta);


		Vec3d operator -(Vec3d v);
		Vec3d operator +(Vec3d v);
		Vec3d operator *(double n);
		Vec3d operator *(Vec3d v);

		void operator +=(Vec3d v);

		~Vec3d();
	};
}

#endif