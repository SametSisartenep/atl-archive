#include <u.h>
#include <libc.h>

enum {
	MULT = 31,
	NHASH = 37,
};

uint
hash(char *s)
{
	uint h;

	h = 0;
	while(*s != 0)
		h = MULT * h + (uchar)*s++;
	return h % NHASH;
}

uint
djb2(char *s)
{
	uint h;

	h = 5381;
	while(*s != 0)
		h = ((h << 5) + h) + (uchar)*s++;
	return h % NHASH;
}

uint
djb2a(char *s)
{
	uint h;

	h = 5381;
	while(*s != 0)
		h = ((h << 5) + h) ^ (uchar)*s++;
	return h % NHASH;
}

uint
fnv1(char *s)
{
	uint h;

	h = 0x811c9dc5;
	while(*s != 0)
		h = (h*0x1000193) ^ (uchar)*s++;
	return h % NHASH;
}

uint
fnv1a(char *s)
{
	uint h;

	h = 0x811c9dc5;
	while(*s != 0)
		h = (h^(uchar)*s++) * 0x1000193;
	return h % NHASH;
}

void
usage(void)
{
	fprint(2, "usage: %s word\n", argv0);
	exits("usage");
}

void
main(int argc, char *argv[])
{
	ARGBEGIN{
	default: usage();
	}ARGEND;
	if(argc != 1)
		usage();
	print("tpop:\t%ud\n", hash(*argv));
	print("djb2:\t%ud\n", djb2(*argv));
	print("djb2a:\t%ud\n", djb2a(*argv));
	print("fnv1:\t%ud\n", fnv1(*argv));
	print("fnv1a:\t%ud\n", fnv1a(*argv));
	exits(0);
}
