#include <u.h>
#include <libc.h>

void
main()
{
	int	cfd;
	char	dir[40];

	cfd = announce("tcp!*!8899", dir);
	if(cfd < 0)
		sysfatal("announce: %r");
	print("announced in %s\n", dir);
	for(;;)
		sleep(1000);
}
