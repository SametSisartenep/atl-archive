#include <u.h>
#include <libc.h>
#include <bio.h>

#define DEFGREET	"Welcome to Antares Labs Computer Bulletin Board Sytem\r\n\r\n"
#define BAUDRATE	300

Biobuf	bin, bout;
char	*wdir = "/sys/lib/bbs";
char	username[32];
int	debug;

void
usage(void)
{
	fprint(2, "usage: bbs [-d] [-m pwd]\n");
	exits("usage");
}

void
reply(char *msg, ulong n)
{
	char	*s;

	for(s = msg; (s-msg) < n; s++){
		write(1, s, 1);
		sleep(1000/(BAUDRATE/8));
	}
}

void
greet(void)
{
	char	buf[1024];
	int	fd, n;

	fd = open("greeting", OREAD);
	if(fd < 0)
		reply(DEFGREET, strlen(DEFGREET));
	else
		while((n = read(fd, buf, sizeof buf)) > 0)
			reply(buf, n);
	close(fd);

	reply(ctime(time(0)), 30-1);
}

int
login(void)
{
	char	*line;
	int	len;

	char	*msg = "hi there, %s. we're currently working on this service.\r\n"
			"sorry for any inconvenience.\r\n"
			"cheers!\r\n"
			"\t\t\t-rodri\r\n\r\n";

	reply("login: ", 7);
	while(line = Brdline(&bin, '\n')){
		len = Blinelen(&bin);
		line[len-2] = '\0';
		strncpy(username, line, sizeof username);
		msg = smprint(msg, username);
		reply(msg, strlen(msg));
		free(msg);
		return -1;
	}
	return 0;
}

void
main(int argc, char *argv[])
{
	char	*line;
	int	len, i;

	ARGBEGIN{
	case 'm':
		wdir = EARGF(usage());
		break;
	case 'd':
		debug = 1;
		break;
	}ARGEND;

	Binit(&bin, 0, OREAD);
	Binit(&bout, 1, OWRITE);

	if(debug)
		for(i = 0; i < argc; i++)
			fprint(2, "arg%d: %s\n", i, argv[i]);

	if(chdir(wdir) < 0)
		sysfatal("chdir: %r");

	greet();

	if(login() < 0){
		fprint(2, "login failed for %s\n", username);
		exits(0);
	}
	while(line = Brdline(&bin, '\n')){
		if(strcmp(line, "q") == 0){
			print("bye!\n");
			exits(0);
		}
		len = Blinelen(&bin);
		Bwrite(&bout, line, len);
		Bflush(&bout);
	}

	Bterm(&bin);
	Bterm(&bout);
	exits(0);
}
