/**
 * nemo's 9.intro
 * ch2, ex2
 */
#include <u.h>
#include <libc.h>

void
usage(void)
{
	fprint(2, "usage: %s [-abc] x y z\n", argv0);
	exits("usage");
}

void
main(int argc, char *argv[])
{
	int	aflag, bflag, cflag;
		aflag = bflag = cflag = 0;

	ARGBEGIN{
	case 'a': aflag = 1; break;
	case 'b': bflag = 1; break;
	case 'c': cflag = 1; break;
	default: usage();
	}ARGEND;

	if(argc < 3)
		usage();

	if(aflag)
		putenv("opta", "yes");
	if(bflag)
		putenv("optb", "yes");
	if(cflag)
		putenv("optc", "yes");

	putenv("args", smprint("%s %s %s", argv[0], argv[1], argv[2]));

	exits(nil);
}
