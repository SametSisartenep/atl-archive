#include <u.h>
#include <libc.h>

static char s[] = "hello ";
char t[] = "world\n";

void
greet(void)
{
	write(1, s, 6);
	write(1, t, 6);
}

void
main()
{
	greet();
	//exits(0);
}
