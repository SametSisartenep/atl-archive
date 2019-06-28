#include <u.h>
#include <libc.h>
#include "../geometry.h"

Quaternion
Quat(double r, double i, double j, double k)
{
	return (Quaternion){r, i, j, k};
}

Quaternion
Quatvec(double s, Point3 v)
{
	return (Quaternion){s, v.x, v.y, v.z};
}

Quaternion
addq(Quaternion a, Quaternion b)
{
	return (Quaternion){a.r+b.r, a.i+b.i, a.j+b.j, a.k+b.k};
}

Quaternion
subq(Quaternion a, Quaternion b)
{
	return (Quaternion){a.r-b.r, a.i-b.i, a.j-b.j, a.k-b.k};
}

Quaternion
mulq(Quaternion q, Quaternion r)
{
	Point3 v1, v2, tmp;

	v1 = Vec3(q.i, q.j, q.k);
	v2 = Vec3(r.i, r.j, r.k);
	tmp = addpt3(addpt3(mulpt3(v2, q.r), mulpt3(v1, r.r)), crossvec3(v1, v2));
	return (Quaternion){q.r*r.r - dotvec3(v1, v2), tmp.x, tmp.y, tmp.z};
}

Quaternion
invq(Quaternion q)
{
	double len²;

	len² = q.r*q.r + q.i*q.i + q.j*q.j + q.k*q.k;
	if(len² == 0)
		return (Quaternion){0, 0, 0, 0};
	return (Quaternion){q.r /= len², q.i = -q.i/len², q.j = -q.j/len², q.k = -q.k/len²};
}

double
qlen(Quaternion q)
{
	return sqrt(q.r*q.r + q.i*q.i + q.j*q.j + q.k*q.k);
}
