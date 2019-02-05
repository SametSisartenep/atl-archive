#include <u.h>
#include <libc.h>

void
usage(void)
{
	fprint(2, "usage: %s [-b bufsz] infile outfile\n", argv0);
	exits("usage");
}

void
main(int argc, char *argv[])
{
	char	*buf;
	long	nr, bufsiz = 8*1024;
	int	ifd, ofd;

	ARGBEGIN{
	case 'b':
		bufsiz = atoi(EARGF(usage()));
		break;
	default:
		usage();
	}ARGEND;

	if(argc < 2)
		usage();

	buf = malloc(bufsiz);
	if(buf == nil)
		sysfatal("no more memory!");

	ifd = open(argv[0], OREAD);
	if(ifd < 0)
		sysfatal("open: %r");
	ofd = create(argv[1], OWRITE, 0644);
	if(ofd < 0)
		sysfatal("open: %r");

	for(;;){
		nr = read(ifd, buf, bufsiz);
		if(nr <= 0)
			break;
		write(ofd, buf, nr);
	}

	close(ifd);
	close(ofd);

	exits(nil);
}
