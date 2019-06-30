#include <u.h>
#include <libc.h>

void
main()
{
	char s[] = ",.";
	char *p;

	p = s;
	while(*p++ == '.' || *p == '.')
		print("dot");
	exits(0);
}
