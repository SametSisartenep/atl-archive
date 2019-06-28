#include <u.h>
#include <libc.h>
#include <bio.h>

void
main()
{
	char	*line;
	int	len;
	Biobuf	bin, bout;

	Binit(&bin, 0, OREAD);
	Binit(&bout, 1, OWRITE);

	while(line = Brdline(&bin, '\n')){
		len = Blinelen(&bin);
		Bwrite(&bout, line, len);
		/* Bflush(&bout); */
	}

	Bterm(&bin);
	Bterm(&bout);

	exits(nil);
}
