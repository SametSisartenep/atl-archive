#include <u.h>
#include <libc.h>

enum {
	Ts,
	Tp,
	Ta,
	Tend
};

typedef struct Vec Vec;
struct Vec {
	double x, y, z;
};

typedef double Veca[3];

vlong t[Tend], t0;

Vec
addvec(Vec a, Vec b)
{
	return (Vec){a.x+b.x, a.y+b.y, a.z+b.z};
}

void
addvecp(Vec *a, Vec *b)
{
	a->x += b->x;
	a->y += b->y;
	a->z += b->z;
}

void
addveca(Veca a, Veca b)
{
	a[0] += b[0];
	a[1] += b[1];
	a[2] += b[2];
}

void
main()
{
	Vec a = {5, 2, 19};
	Vec b = {213, 30, -12};
	Veca aa = {2, 9, -1};
	Veca ab = {-10, 20, 30};

	t0 = nsec();
	a = addvec(a, b);
	t[Ts] = nsec()-t0;
	t0 = nsec();
	addvecp(&a, &b);
	t[Tp] = nsec()-t0;
	t0 = nsec();
	addveca(aa, ab);
	t[Ta] = nsec()-t0;
	print("struct\t%lldns\nstruct*\t%lldns\narray\t%lldns\n", t[Ts], t[Tp], t[Ta]);
	exits(nil);
}
