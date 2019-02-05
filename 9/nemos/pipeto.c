#include <u.h>
#include <libc.h>

int
pipeto(char *cmd)
{
	int	pfd[2];

	if(pipe(pfd) < 0)
		sysfatal("can't get pipe");
	switch(fork()){
	case -1: return -1;
	case 0:
		close(pfd[1]);
		dup(pfd[0], 0);
		close(pfd[0]);
		execl("/bin/rc", "rc", "-c", cmd, nil);
		sysfatal("exec: %r");
	default:
		close(pfd[0]);
		return pfd[1];
	}
}

void
main()
{
	int	fd, i;
	char	*msgs[] = {
		"warning: armaggedon incoming\n",
		"spam: how could you live without us?\n",
		"warning: said benefit is uncertain\n"
	};

	fd = pipeto("grep warning");
	if(fd < 0)
		sysfatal("pipeto: %r");
	for(i = 0; i < nelem(msgs); i++)
		write(fd, msgs[i], strlen(msgs[i]));
	close(fd);
	waitpid();
	exits(0);
}