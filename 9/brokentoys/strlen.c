#include <u.h>
#include <libc.h>

void
main()
{
	char txt[] = "i'm here now\n";

	print("len: %ld, lastchar: %c\n", strlen(txt), txt[strlen(txt)-1]);
	exits(nil);
}
