#include <u.h>
#include <libc.h>

void
main()
{
	int	acfd, lcfd, dfd;
	char	adir[40], ldir[40];
	int	n;
	char	buf[128];

	acfd = announce("tcp!*!8899", adir);
	if(acfd < 0)
		sysfatal("announce: %r");
	for(;;){
		lcfd = listen(adir, ldir);
		if(lcfd < 0)
			sysfatal("listen: %r");
		switch(fork()){
		case -1: fprint(2, "couldn't fork\n"); break;
		case 0:
			dfd = accept(lcfd, ldir);
			if(dfd < 0)
				sysfatal("accept: %r");
			print("accepted call at %s\n", ldir);
			while((n = read(dfd, buf, sizeof buf)) > 0)
				write(dfd, buf, n);
			print("terminated call at %s\n", ldir);
			exits(0);
		default:
			close(lcfd);
		}
	}
}
