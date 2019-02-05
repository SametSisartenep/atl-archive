#include <u.h>
#include <libc.h>
#include <bio.h>

void
main(int argc, char *argv[])
{
	char	*line;
	int	i, len;
	Biobuf	*bin, *bout;

	bout = Bopen("/fd/1", OWRITE);
	if(bout == nil)
		sysfatal("Bopen: %r");

	if(argc < 2){
		bin = Bopen("/fd/0", OREAD);
		if(bin == nil)
		sysfatal("Bopen: %r");
		while(line = Brdline(bin, '\n')){
			len = Blinelen(bin);
			Bwrite(bout, line, len);
		}
		Bterm(bin);
	}else
		for(i = 1; i < argc; i++){
			bin = Bopen(argv[i], OREAD);
			if(bin == nil)
				sysfatal("Bopen: %r");
			while(line = Brdline(bin, '\n')){
				len = Blinelen(bin);
				Bwrite(bout, line, len);
			}
			Bterm(bin);
		}

	Bterm(bout);

	exits(nil);
}
