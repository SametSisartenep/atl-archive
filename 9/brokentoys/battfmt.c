#include <u.h>
#include <libc.h>
#include <ctype.h>

void
battfmt(char *buf, long buflen)
{
	char *s;

	for(s = buf; (s-buf) < buflen; s++)
		if(!isdigit(*s)){
			for(;(s-buf) < buflen; s++)
				*s = '\0';
			break;
		}
}

void
main(void)
{
	char str[] = "30 m";

	battfmt(str, 4);

	print("%s\n", str);

	exits("done");
}
