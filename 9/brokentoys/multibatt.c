#include <u.h>
#include <libc.h>

char f[] = "/mnt/acpi/battery";
char buf[512];
char *s;
int nb = 0;

void
main(void)
{
	int fd;

	fd = open(f, OREAD);
	if(fd < 0)
		sysfatal("open: %r");

	read(fd, buf, sizeof buf);

	buf[strlen(buf)-1] = '\0';

	print("%d\n", atoi(buf)); nb++;
	for(s = buf; *s != '\0'; s++)
		if(*s == '\n'){
			nb++;
			print("%d\n", atoi(++s));
		}
	print("batteries: %d\n", nb);
	exits("done");
}
