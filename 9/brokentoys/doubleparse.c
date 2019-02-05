#include <u.h>
#include <libc.h>
#include <ctype.h>

void
main()
{
	char *s, buf[512];
	double n[2];

	read(0, buf, sizeof(buf)-1);
	buf[sizeof(buf)-1] = 0;
	s = buf;
	n[0] = strtod(s, &s);
	while(isspace(*++s))
		;
	n[1] = strtod(s, &s);
	print("n0: %g\nn1: %g\n", n[0], n[1]);
	exits(0);
}
