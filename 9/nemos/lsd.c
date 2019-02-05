#include <u.h>
#include <libc.h>

void
usage(void)
{
	fprint(2, "usage: lsd dir\n");
	exits("usage");
}

void
main(int argc, char *argv[])
{
	Dir	*dents;
	int	ndents, fd, i;

	if(argc < 2)
		usage();
	fd = open(argv[1], OREAD);
	if(fd < 0)
		sysfatal("open: %r");
	for(;;){
		ndents = dirread(fd, &dents);
		if(ndents == 0)
			break;
		for(i = 0; i < ndents; i++)
			print("%s\n", dents[i].name);
		free(dents);
	}
	exits(nil);
}
