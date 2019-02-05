#include <u.h>
#include <libc.h>
#include <draw.h>
#include "dat.h"
#include "fns.h"

Vector3
Vec3(double x, double y, double z)
{
	return (Vector3){x, y, z};
}

Vector3
addvec3(Vector3 v, Vector3 u)
{
	return (Vector3){v.x+u.x, v.y+u.y, v.z+u.z};
}

Vector3
subvec3(Vector3 v, Vector3 u)
{
	return (Vector3){v.x-u.x, v.y-u.y, v.z-u.z};
}

Vector3
mulvec3(Vector3 v, double s)
{
	return (Vector3){v.x*s, v.y*s, v.z*s};
}

double
dotvec3(Vector3 v, Vector3 u)
{
	return v.x*u.x + v.y*u.y + v.z*u.z;
}

Vector3
crossvec(Vector3 v, Vector3 u)
{
	return (Vector3){
		v.y*u.z - v.z*u.y,
		v.z*u.x - v.x*u.z,
		v.x*u.y - v.y*u.x
	};
}

Vector3
normvec3(Vector3 v)
{
	double len;

	len = hypot3(v.x, v.y, v.z);
	if(len == 0)
		return (Vector3){0, 0, 0};
	v.x /= len;
	v.y /= len;
	v.z /= len;
	return v;
}
