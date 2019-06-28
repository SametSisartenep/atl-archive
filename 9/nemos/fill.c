#include <u.h>
#include <libc.h>

void
main()
{
	char	buf[1024];
	int	fd[2], nw, total = 0;

	if(pipe(fd)<0)
		sysfatal("can't get pipe");
	for(;;){
		nw = write(fd[0], buf, sizeof buf);
		total += nw;
		print("%d bytes written so far\n", total);
	}
}
