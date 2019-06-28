#include <u.h>
#include <libc.h>

void
main()
{
	char	buf[128];
	int	fd[2], nr;

	if(pipe(fd) < 0)
		sysfatal("couldn't pipe");
	write(fd[1], "Hello!\n", 7);
	write(fd[1], "there\n", 6);
	while((nr = read(fd[0], buf, sizeof buf)) > 0)
		write(1, buf, nr);
		/* print("[%d]\n", nr); */
	exits(0);
}
