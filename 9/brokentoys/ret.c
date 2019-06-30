#include <u.h>
#include <libc.h>

int
ret(void)
{
	if(2 > 3)
		return 2;
	else
		exits("error");
	return -1;
}

void
main()
{
	ret();
	exits(0);
}
