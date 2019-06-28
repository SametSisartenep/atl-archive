#include <u.h>
#include <libc.h>

void
usage(void)
{
	fprint(2, "usage: serv netaddr\n");
	exits("usage");
}

void
main(int argc, char *argv[])
{
	int	fd, sfd;
	char	*addr;
	char	fname[128];

	if(argc < 2)
		usage();

	addr = netmkaddr(argv[1], "tcp", "9fs");
	fd = dial(addr, nil, nil, nil);
	if(fd < 0)
		sysfatal("dial: %s: %r", addr);
	seprint(fname, fname+sizeof(fname), "/srv/%s", argv[1]);
	sfd = create(fname, OWRITE, 0664);
	if(sfd < 0)
		sysfatal("open: %r");
	fprint(sfd, "%d", fd);
	close(sfd);
	close(fd);
	exits(0);
}
