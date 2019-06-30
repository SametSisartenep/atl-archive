#include <u.h>
#include <libc.h>

void
main()
{
	write(1, "Hello world\n", 12);
	exits(0);
}
