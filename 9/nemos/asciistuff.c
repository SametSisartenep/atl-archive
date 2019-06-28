#include <u.h>
#include <libc.h>

void
main()
{
	int	c;
	for(c = 97; c <= 'z'; c++)
		print("%c\n", c);
	exits(nil);
}
