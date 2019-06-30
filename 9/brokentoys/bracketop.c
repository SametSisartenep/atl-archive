#include <u.h>
#include <libc.h>

void
main()
{
	int a[] = {1, 2};

	print("%d\n", 1[a]);
	exits(nil);
}
