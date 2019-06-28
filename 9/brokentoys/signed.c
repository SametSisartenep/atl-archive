#include <u.h>
#include <libc.h>

void
main()
{
	int x;

	x = 25;
	print("n:%d,s:%d\n", x, (x>>31));
	x *= -1;
	print("n:%d,s:%d\n", x, (x>>31));
	exits(0);
}
