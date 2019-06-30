#include <u.h>
#include <libc.h>

char *p = "gooseberry";
char a[] = "gooseberry";

void
main()
{
	print("%s\n%s\n", p, a);
	strncpy(p, "black", 5);
	strncpy(a, "black", 5);
	print("%s\n%s\n", p, a);
	exits(nil);
}
