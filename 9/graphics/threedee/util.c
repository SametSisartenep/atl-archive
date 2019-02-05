#include <u.h>
#include <libc.h>
#include <draw.h>
#include "dat.h"
#include "fns.h"

double
round(double n)
{
	return floor(n + 0.5);
}

Point
rotatept(Point p, double θ, Point c)
{
	Point r;

	p = subpt(p, c);
	r.x = round(p.x*cos(θ) - p.y*sin(θ));
	r.y = round(p.x*sin(θ) + p.y*cos(θ));
	r = addpt(r, c);
	return r;
}

double
hypot3(double x, double y, double z)
{
	return hypot(x, hypot(y, z));
}
