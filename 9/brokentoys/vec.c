#include <u.h>
#include <libc.h>

typedef struct Vec Vec;
struct Vec {
	double x, y, z;
};

Vec
addvec(Vec a, Vec b)
{
	a.x += b.x;
	a.y += b.y;
	a.z += b.z;
	return a;
}

void
main()
{
	Vec a = {20, 5, 3};
	Vec b = {6, -2, 19};

	a = addvec(a, b);
	exits(nil);
}
