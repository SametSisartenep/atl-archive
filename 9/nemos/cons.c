#include <u.h>
#include <libc.h>

void
main()
{
	char buf[1024];
	int fd;
	long nr;

	fd = open("/dev/cons", ORDWR);
	if(fd < 0)
		sysfatal("open: %r");

	nr = read(fd, buf, sizeof buf);
	write(1, buf, nr);

	close(fd);

	exits(nil);
}
