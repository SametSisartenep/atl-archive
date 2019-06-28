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
	write(fd[0], "Hi\n", 3);
	nr = read(fd[0], buf, sizeof buf);
	write(1, buf, nr);
	nr = read(fd[1], buf, sizeof buf);
	write(1, buf, nr);
	exits(0);
}
