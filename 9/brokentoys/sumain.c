#include <u.h>
#include <libc.h>

int sum(int, int);

void
main()
{
	int n;

	n = sum(24, 30);
	print("Σ = %d\n", n);
	exits(nil);
}
