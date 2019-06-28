#include <u.h>
#include <libc.h>

void
fooi(void)
{
	static int	x = 5;
	x++;
	print("fooi: %d\n", x);
}

void
foo(void)
{
	static int	x;
	x = 5;
	x++;
	print("foo: %d\n", x);
}

void
bar(void)
{
	int	x = 3;
	x++;
	print("bar: %d\n", x);
}

void
main()
{
	fooi();
	fooi();
	foo();
	foo();
	bar();
	bar();
	exits(0);
}
