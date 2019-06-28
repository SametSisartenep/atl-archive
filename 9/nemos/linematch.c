#include <u.h>
#include <libc.h>
#include <regexp.h>

void
usage(void)
{
	fprint(2, "usage: linematch expr\n");
	exits("usage");
}

void
main(int argc, char *argv[])
{
	Reprog *prog;
	Resub sub[16];
	char buf[1024];
	int nr, matches;

	if(argc < 2)
		usage();

	prog = regcomp(argv[1]);
	if(!prog)
		sysfatal("regexp '%s': %r", argv[1]);
	for(;;){
		nr = read(0, buf, sizeof(buf)-1);
		if(nr <= 0)
			break;
		buf[nr] = 0;
		matches = regexec(prog, buf, sub, nelem(sub));
		if(matches){
			print("matched: '");
			write(1, sub[0].sp, sub[0].ep-sub[0].sp);
			print("'\n");
		}else
			print("no matches\n");
	}
	exits(nil);
}
