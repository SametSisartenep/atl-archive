#include <u.h>
#include <libc.h>

void
main()
{
	if(0.00001 > 0) print("ok\n");
	else print("ko\n");
	exits(0);
}
