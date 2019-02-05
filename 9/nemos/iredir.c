#include <u.h>
#include <libc.h>

void
main()
{
	int	fd;

	switch(fork()){
	case -1: sysfatal("fork failed");
	case 0:
		fd = open("/lib/legal/NOTICE", OREAD);
		dup(fd, 0);
		close(fd);
		execl("/bin/cat", "cat", nil);
		sysfatal("exec failed");
	default: waitpid();
	}
	exits(0);
}
