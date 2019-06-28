#include <u.h>
#include <libc.h>

double
hypot3(double x, double y, double z)
{
	return hypot(hypot(x, y), z);
}
