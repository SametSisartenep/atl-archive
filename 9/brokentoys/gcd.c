#include <u.h>
#include <libc.h>

enum {
	Tgcd,
	Tmod,
	Te
};
vlong t[Te];
vlong t0;

void
swap(int *a, int *b)
{
	int tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}

int
gcd(int u, int v)
{
	while(u > 0){
		if(u < v)
			swap(&u, &v);
		u = u-v;
	}
	return v;
}

int
modgcd(int u, int v)
{
	while(u > 0){
		if(u < v)
			swap(&u, &v);
		u = u % v;
	}
	return v;
}

void
usage(void)
{
	fprint(2, "usage: gcd num den\n");
	exits("usage");
}

void
main(int argc, char *argv[])
{
	int n, m;

	if(argc != 3)
		sysfatal("not enough arguments");
	n = strtol(argv[1], nil, 0);
	m = strtol(argv[2], nil, 0);
	t0 = nsec();
	print("GCD %d %d = %d\n", n, m, gcd(n, m));
	t[Tgcd] = nsec()-t0;
	t0 = nsec();
	print("MODGCD %d %d = %d\n", n, m, modgcd(n, m));
	t[Tmod] = nsec()-t0;
	print("\tgcd: %lld | mod: %lld\n", t[Tgcd], t[Tmod]);
	exits(nil);
}
