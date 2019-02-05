#include <u.h>
#include <libc.h>
#include <draw.h>
#include "dat.h"
#include "fns.h"

Point
centroid(Triangle t)
{
	return divpt(addpt(t.p0, addpt(t.p1, t.p2)), 3);
}

void
triangle(Image *dst, Triangle t, Image *src, Point p)
{
	Point pl[4];

	pl[0] = tobase(t.p0);
	pl[1] = tobase(t.p1);
	pl[2] = tobase(t.p2);
	pl[3] = pl[0];

	poly(dst, pl, nelem(pl), 0, 0, 0, src, p);
}

void
filltriangle(Image *dst, Triangle t, Image *src, Point p)
{
	Point pl[4];

	pl[0] = tobase(t.p0);
	pl[1] = tobase(t.p1);
	pl[2] = tobase(t.p2);
	pl[3] = pl[0];

	fillpoly(dst, pl, nelem(pl), 0, src, p);
}

Triangle
gentriangle(void)
{
	Triangle t;

	t.p0 = Pt(ntruerand(Dx(screen->r)), ntruerand(Dy(screen->r)));
	t.p1 = Pt(ntruerand(Dx(screen->r)), ntruerand(Dy(screen->r)));
	t.p2 = Pt(ntruerand(Dx(screen->r)), ntruerand(Dy(screen->r)));
	return t;
}

Triangle
rotatriangle(Triangle t, double θ)
{
	Triangle r;

	r.p0.x = t.p0.x*cos(θ) - t.p0.y*sin(θ);
	r.p0.y = t.p0.x*sin(θ) + t.p0.y*cos(θ);
	r.p1.x = t.p1.x*cos(θ) - t.p1.y*sin(θ);
	r.p1.y = t.p1.x*sin(θ) + t.p1.y*cos(θ);
	r.p2.x = t.p2.x*cos(θ) - t.p2.y*sin(θ);
	r.p2.y = t.p2.x*sin(θ) + t.p2.y*cos(θ);
	return r;
}

Triangle
inrotatriangle(Triangle t, double θ)
{
	Triangle r;
	Point c;

	c = centroid(t);
	t.p0 = subpt(t.p0, c);
	t.p1 = subpt(t.p1, c);
	t.p2 = subpt(t.p2, c);
	r.p0.x = t.p0.x*cos(θ) - t.p0.y*sin(θ);
	r.p0.y = t.p0.x*sin(θ) + t.p0.y*cos(θ);
	r.p1.x = t.p1.x*cos(θ) - t.p1.y*sin(θ);
	r.p1.y = t.p1.x*sin(θ) + t.p1.y*cos(θ);
	r.p2.x = t.p2.x*cos(θ) - t.p2.y*sin(θ);
	r.p2.y = t.p2.x*sin(θ) + t.p2.y*cos(θ);
	r.p0 = addpt(r.p0, c);
	r.p1 = addpt(r.p1, c);
	r.p2 = addpt(r.p2, c);
	return r;
}
