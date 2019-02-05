#include <u.h>
#include <libc.h>

void *
emalloc(ulong n)
{
	void *p;

	p = malloc(n);
	if(p == nil)
		sysfatal("malloc: %r");
	memset(p, 0, n);
	setmalloctag(p, getcallerpc(&n));
	return p;
}

char **listp;
char *lista[] = {
	"just",
	"a",
	"couple",
	"of",
	"words"
};
char s[] = "ignore this and keep going";

void
main()
{
	print("listp size pre-alloc: %d\n", sizeof(listp));
	listp = emalloc(4096);
	print("listp size post-alloc: %d\n", sizeof(listp));
	print("lista size: %d & len: %d\n", sizeof(lista), nelem(lista));
	print("s size: %d & len: %ld\n", sizeof(s), strlen(s));
	exits(0);
}
