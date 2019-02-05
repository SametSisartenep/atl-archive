#include <u.h>
#include <libc.h>

#define	MAXREADS	3

void
main(void)
{
	int	fd, i;
	char	buf[4];

	fd = open("/mnt/acpi/battery", OREAD);
	if(fd < 0)
		sysfatal("couldn't open it");

	for(i = 0; i < MAXREADS; i++){
		pread(fd, buf, sizeof buf, 0);
		buf[sizeof(buf)-1] = '\0';
		print(buf);
	}

	close(fd);
	exits("done");
}
