#include <u.h>
#include <libc.h>

void
main()
{
	int	pfd[2], fd, nr;
	char	buf[1024];

	if(pipe(pfd) < 0)
		sysfatal("can't get pipe");
	fd = create("/srv/echo", OWRITE, 0664);
	if(fd < 0)
		sysfatal("couldn't create at /srv: %r");
	if(fprint(fd, "%d", pfd[1]) < 0)
		sysfatal("couldn't post fd: %r");
	close(pfd[1]);
	for(;;){
		nr = read(pfd[0], buf, sizeof buf);
		if(nr <= 0)
			break;
		write(1, buf, nr);
	}
	exits(0);
}
