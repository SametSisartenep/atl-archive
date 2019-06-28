#include <u.h>
#include <libc.h>

void
main()
{
	int	i;

	switch(rfork(RFNOTEG|RFPROC)){
	case -1: sysfatal("fork failed");
	case 0:
		/* best implementation of http ever. */
		for(i = 0; i < 5; i++){
			sleep(1000);
			print("%d\n", i);
		}
	default: break;
	}
	exits(0);
}
