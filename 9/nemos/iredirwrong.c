#include <u.h>
#include <libc.h>

void
main()
{
	int	fd;

	switch(fork()){
	case -1: sysfatal("fork failed");
	case 0:
		fd = create("afile", OWRITE, 0644);
		dup(fd, 0);
		close(fd);
		execl("/bin/cat", "cat", nil);
		sysfatal("exec: %r");
	default: waitpid();
	}
	exits(0);
}
