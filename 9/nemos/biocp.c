#include <u.h>
#include <libc.h>
#include <bio.h>

void
usage(void)
{
	fprint(2, "usage: %s [-b bufsz] infile outfile\n", argv0);
	exits("usage");
}

void*
emalloc(ulong n)
{
	void	*v;

	v = malloc(n);
	if(v == nil)
		sysfatal("malloc: %r");

	setmalloctag(v, getcallerpc(&n));

	return v;
}

void
main(int argc, char *argv[])
{
	char	*buf;
	int	nr, bufsz = 8*1024;
	Biobuf	*bin, *bout;

	ARGBEGIN{
	case 'b':
		bufsz = atoi(EARGF(usage()));
		break;
	default:
		usage();
	}ARGEND;

	if(argc < 2)
		usage();

	buf = emalloc(bufsz);
	bin = Bopen(argv[0], OREAD);
	if(bin == nil)
		sysfatal("Bopen: %r");
	bout = Bopen(argv[1], OWRITE);
	if(bout == nil)
		sysfatal("Bopen: %r");

	for(;;){
		nr = Bread(bin, buf, bufsz);
		if(nr <= 0)
			break;
		Bwrite(bout, buf, nr);
	}

	Bterm(bin);
	Bterm(bout);

	exits(nil);
}
