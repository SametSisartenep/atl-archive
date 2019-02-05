#include <u.h>
#include <libc.h>

void
main()
{
	int	fd[2];

	if(pipe(fd) < 0)
		sysfatal("can't get pipe");

	switch(fork()){
	case -1: sysfatal("fork failed");
	case 0:
		close(fd[0]);
		dup(fd[1], 1);
		close(fd[1]);
		execl("/bin/ls", "ls", nil);
		sysfatal("exec: %r");
	default:
		close(fd[1]);
		dup(fd[0], 0);
		close(fd[0]);
		execl("/bin/grep", "grep", "bi", nil);
		sysfatal("exec: %r");
	}
	exits(0);
}
