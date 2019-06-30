#include <u.h>
#include <libc.h>

typedef struct Node Node;
struct Node {
	int n;
	Node *next;
};

Node *head, *tail;

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

void
put(int n)
{
	if(tail == nil){
		tail = emalloc(sizeof(Node));
		tail->n = n;
		head = tail;
		return;
	}
	tail->next = emalloc(sizeof(Node));
	tail = tail->next;
	tail->n = n;
}

int
get(void)
{
	Node *nn;
	int n;

	if(head == nil)
		return -1;
	nn = head->next;
	n = head->n;
	free(head);
	head = nn;
	return n;
}

int
isemtpy(void)
{
	return head == nil;
}

void
main()
{
	int i;

	for(i = 0; i < 10; i++)
		put(i*3);
	while(!isemtpy())
		print("%d\n", get());
	exits(nil);
}
