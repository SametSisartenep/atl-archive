#include <u.h>
#include <libc.h>

double
hypot2(double p, double q)
{
	return sqrt(p*p + q*q);
}

double
hypot3(double x, double y, double z)
{
	return sqrt(x*x + y*y + z*z);
}

double
hypot3from2(double x, double y, double z)
{
	return hypot(hypot(x, z), y);
}

void
usage(void)
{
	fprint(2, "usage: hypotenuse x y z\n");
	exits("usage");
}

void
main(int argc, char *argv[])
{
	double x, y, z, r;
	vlong t0, t;

	if(argc < 4)
		usage();
	x = strtod(argv[1], nil);
	y = strtod(argv[2], nil);
	z = strtod(argv[3], nil);
	/*print("\t2D\n");
	t0 = nsec();
	r = hypot2(x, y);
	t = nsec();
	print("1st method: %g (%lld ns)\n", r, t-t0);
	t0 = nsec();
	r = hypot(x, y);
	t = nsec();
	print("2nd method: %g (%lld ns)\n", r, t-t0);
	print("\t3D\n");
	t0 = nsec();
	r = hypot3(x, y, z);
	t = nsec();
	print("1st method: %g (%lld ns)\n", r, t-t0);*/
	t0 = nsec();
	r = hypot3from2(x, y, z);
	t = nsec();
	print("2nd method: %g (%lld ns)\n", r, t-t0);
	exits(0);
}
