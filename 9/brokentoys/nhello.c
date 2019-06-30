#include <u.h>
#include <libc.h>

char c;
int n;

void
main()
{
	read(0, &c, 1);
	n = c-'0';
	while(n-- > 0)
		write(1, "Hola mundo.\n", 12);
	exits(0);
}
