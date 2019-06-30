#include <u.h>
#include <libc.h>
#include <ctype.h>
#include <bio.h>

int stk[256], sp;
Biobuf *bin;

void
push(int n)
{
	if(sp >= sizeof stk)
		sp = sizeof(stk)-1;
	fprint(2, "push [%d] %d\n", sp, n);
	stk[sp++] = n;
}

int
pop(void)
{
	if(sp <= 0)
		sp = 1;
	fprint(2, "pop [%d] %d\n", sp-1, stk[sp-1]);
	return stk[--sp];
}

void
main()
{
	int x;
	char c;

	bin = Bfdopen(0, OREAD);
	if(bin == nil)
		sysfatal("Bfdopen: %r");
	while((c = Bgetc(bin)) != Beof){
		x = 0;
		switch(c){
		case '+': x = pop() + pop(); break;
		case '*': x = pop() * pop(); break;
		default:
			while(isdigit(c)){
				x = x*10 + c-'0';
				c = Bgetc(bin);
			}
		}
		push(x);
	}
	print("%d\n", pop());
	exits(nil);
}
