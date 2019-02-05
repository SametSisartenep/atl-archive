#include <u.h>
#include <libc.h>

void
main()
{
	switch(rfork(RFFDG|RFREND|RFPROC)){
	case -1: sysfatal("fork failed");
	case 0:
		execl("/bin/ls", "ls", nil);
		sysfatal("exec: %r");	
	default:
		waitpid();
	}
	exits(0);
}
