#include <u.h>
#include <libc.h>

void
main()
{
	switch(fork()){
	case -1: sysfatal("fork failed");
	case 0: execl("/bin/ls", "ls", nil); break;
	default: print("ls running\n");
	}
	exits(0);
}
