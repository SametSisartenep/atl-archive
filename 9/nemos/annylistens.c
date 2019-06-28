#include <u.h>
#include <libc.h>

void
main()
{
	int	acfd, lcfd;
	char	adir[40], ldir[40];

	acfd = announce("tcp!*!8899", adir);
	if(acfd < 0)
		sysfatal("announce: %r");
	print("announced in %s (fd=%d)\n", adir, acfd);
	lcfd = listen(adir, ldir);
	print("attending call in %s (fd=%d)\n", ldir, lcfd);
	for(;;)
		sleep(1000);
}
