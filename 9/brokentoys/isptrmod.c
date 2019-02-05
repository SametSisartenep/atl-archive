#include <u.h>
#include <libc.h>

/*
 * this program proofs that effectively, pointer arguments are passed by
 * value and its modification doesn't extend beyond the scope of the
 * currently running procedure.
 */

void
incptr(char *p)
{
	p += 3;
	print("%s\n", p);
}

void
main()
{
	char *s;

	s = "hi there!";
	print("%s\n", s);
	incptr(s);
	print("%s\n", s);
	exits(0);
}
