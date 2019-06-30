#include <u.h>
#include <libc.h>

void
greet(void)
{
	write(1, "hello world\n", 12);
}

void
main()
{
	greet();
}
