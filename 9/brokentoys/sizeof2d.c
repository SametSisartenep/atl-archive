#include <u.h>
#include <libc.h>

void
main()
{
	char m[10][5];

	print("%d\n", sizeof(m));
	exits(nil);
}
