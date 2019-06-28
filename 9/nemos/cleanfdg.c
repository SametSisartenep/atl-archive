#include <u.h>
#include <libc.h>

void
main()
{
	print("hello\n");
	rfork(RFCFDG);
	print("there\n");
	exits(0);
}
