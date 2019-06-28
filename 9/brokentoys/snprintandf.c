#include <u.h>
#include <libc.h>
#include <stdio.h>

void
main()
{
	char *s;
	int l;

	s = malloc(128);
	l = snprint(s, 128, "%s%s", "hi", "there");
	print("%s(%d)\n", s, l);
	l = snprintf(s, 128, "%s%s", "hi", "there");
	print("%s(%d)\n", s, l);
	free(s);
	exits(0);
}
