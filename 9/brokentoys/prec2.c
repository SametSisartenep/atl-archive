#include <u.h>
#include <libc.h>

void
main()
{
	int n;
	int *p;

	n = 4;
	p = &n;
	print("%d\n", ++*p);
	print("%d\n", n);
	exits(0);
}
