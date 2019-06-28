#include <u.h>
#include <libc.h>

typedef struct Num Num;

struct Num {
	int	v;
};

Num one = {1};
Num three = {3};
Num fourteen = {14};

char *list[] = {
[one.v]		"ein",
[three.v]	"drei",
[fourteen.v]	"vierzehn"
};

void
main()
{
	print("%s\n", list[one.v]);
	exits(0);
}
