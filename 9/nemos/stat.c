#include <u.h>
#include <libc.h>

void
usage(void)
{
	fprint(2, "usage: stat file\n");
	exits("usage");
}

void
main(int argc, char *argv[])
{
	Dir	*d;

	if(argc < 2)
		usage();

	if((d = dirstat(argv[1])) == nil)
		sysfatal("could not stat %s: %r\n", argv[1]);

	print("file name: %s\n", d->name);
	print("file mode: 0%o\n", d->mode);
	print("file size: %lld bytes\n", d->length);

	free(d);

	exits(nil);
}
