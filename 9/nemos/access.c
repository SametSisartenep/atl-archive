#include <u.h>
#include <libc.h>

void
usage(void)
{
	fprint(2, "usage: access file\n");
	exits("usage");
}

void
main(int argc, char *argv[])
{
	if(argc < 2)
		usage();

	if(access(argv[1], AEXIST) < 0)
		sysfatal("%s does not exist", argv[1]);

	exits(nil);
}
