#include <u.h>
#include <libc.h>

void
main()
{
	fork();
	fork();
	print("hi!\n");
}
