#include <u.h>
#include <libc.h>
#include "dat.h"

Vec
addvec(Vec a, Vec b)
{
	return (Vec){a.x+b.x, a.y+b.y};
}

Vec
subvec(Vec a, Vec b)
{
	return (Vec){a.x-b.x, a.y-b.y};
}

Vec
mulvec(Vec v, double s)
{
	return (Vec){v.x*s, v.y*s};
}

double
dotvec(Vec a, Vec b)
{
	return a.x*b.x + a.y*b.y;
}
