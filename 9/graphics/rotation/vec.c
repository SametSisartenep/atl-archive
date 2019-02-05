#include <u.h>
#include <libc.h>
#include <draw.h>
#include "dat.h"
#include "fns.h"

Vec
addvec(Vec v, Vec u)
{
	return (Vec){v.x+u.x, v.y+u.y};
}

Vec
subvec(Vec v, Vec u)
{
	return (Vec){v.x-u.x, v.y-u.y};
}

Vec
mulvec(Vec v, double s)
{
	return (Vec){v.x*s, v.y*s};
}

double
dotvec(Vec v, Vec u)
{
	return v.x*u.x + v.y*u.y;
}
