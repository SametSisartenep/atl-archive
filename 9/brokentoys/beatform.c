#include <u.h>
#include <libc.h>

uvlong
next(uvlong t)
{
	print("t0: %llud ", t);
	t = t * 8000 / 44100;
	print("t1: %llud ", t);
	t = t*(42&t>>10);
	print("t2: %llud ", t);
	print("t3: %llud ", t<<8);
	return t<<8;
}

void
main()
{
	uvlong t;
	short sl;

	for(t = 20000;; t++){
		sl = next(t);
		print("%2d\t", sl);
	}
}
