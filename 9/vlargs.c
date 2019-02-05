#include <u.h>
#include <libc.h>

void
usage(void)
{
	fprint(2, "usage: %s [-abc] args...\n", argv0);
	exits("usage");
}

void
main(int argc, char *argv[])
{
	int	aflag, bflag, cflag;
	int	arglen, i;
	char	*args;

	aflag = bflag = cflag = 0;

	ARGBEGIN{
	case 'a': aflag = 1; break;
	case 'b': bflag = 1; break;
	case 'c': cflag = 1; break;
	default: usage();
	}ARGEND;

	if(aflag || bflag || cflag)
		;

	if(argc < 1)
		usage();

	for(i = 0, arglen = 0; i < argc; i++)
		arglen += strlen(argv[i]) + 1;

	print("we're dealing with %d bytes\n", arglen);

	args = mallocz(arglen, 1);

	strcat(args, argv[0]);
	for(i = 1; i < argc; i++)
		strcat(args, smprint(" %s", argv[i]));

	print("%s\n", args);

	exits(nil);
}
