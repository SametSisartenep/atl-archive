#include <u.h>
#include <libc.h>
#include "dat.h"
#include "fns.h"

Vec
addvec(Vec v, Vec u)
{
	return (Vec){v.x+u.x, v.y+u.y, v.z+u.z};
}

Vec
subvec(Vec v, Vec u)
{
	return (Vec){v.x-u.x, v.y-u.y, v.z-u.z};
}

Vec
mulvec(Vec v, double s)
{
	return (Vec){v.x*s, v.y*s, v.z*s};
}

Vec
normvec(Vec v)
{
	double l;

	l = hypot3(v.x, v.y, v.z);
	if(l == 0)
		return (Vec){0, 0, 0};
	v.x /= l;
	v.y /= l;
	v.z /= l;
	return v;
}

double
dotvec(Vec v, Vec u)
{
	return v.x*u.x + v.y*u.y + v.z*u.z;
}

Vec
crossvec(Vec v, Vec u)
{
	return (Vec){
		v.y*u.z - v.z*u.y,
		v.z*u.x - v.x*u.z,
		v.x*u.y - v.y*u.x
	};
}
