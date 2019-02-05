#include <u.h>
#include <libc.h>

void
main()
{
	char	str1[] = "hello\n\0";

	print("nbytes: %d\nstrlen: %ld\n", sizeof str1, strlen(str1));

	exits(nil);
}
