#include <u.h>
#include <libc.h>

int
handler(void*, char *msg)
{
	print("note: %s\n", msg);
	return 0;
}

void
main()
{
	atnotify(handler, 1);
	sleep(120*1000);
	print("done (%r)");
	exits(0);
}
