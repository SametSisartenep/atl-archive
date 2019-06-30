#include <u.h>
#include <libc.h>
#include <draw.h>

enum { SEC = 1000 };

enum {
	Cred,
	Cgrn,
	Cblu,

	Cgx,
	Cgy,
	Cend
};
Image *col[Cend];

int dx, dy;

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

Image *
eallocimage(Display *d, Rectangle r, ulong chan, int repl, ulong col)
{
	Image *i;

	i = allocimage(d, r, chan, repl, col);
	if(i == nil)
		sysfatal("allocimage: %r");
	return i;
}

void
usage(void)
{
	fprint(2, "usage: colgrad\n");
	exits("usage");
}

void
main(int argc, char *argv[])
{
	int i;
	uchar *gx, *gy;

	ARGBEGIN{
	default: usage();
	}ARGEND;

	if(initdraw(nil, nil, "colgrad") < 0)
		sysfatal("initdraw: %r");
	dx = Dx(screen->r), dy = Dy(screen->r);
	col[Cred] = eallocimage(display, Rect(0, 0, 1, 1), RGB24, 1, DRed);
	col[Cgrn] = eallocimage(display, Rect(0, 0, 1, 1), RGB24, 1, DGreen);
	col[Cblu] = eallocimage(display, Rect(0, 0, 1, 1), RGB24, 1, DBlue);
	col[Cgx] = eallocimage(display, Rect(0, 0, dx, 1), GREY8, 1, DNofill);
	col[Cgy] = eallocimage(display, Rect(0, 0, 1, dy), GREY8, 1, DNofill);
	gx = emalloc(dx);
	gy = emalloc(dy);
	for(i = 0; i < dx; i++)
		gx[i] = 255.0 * i/(dx-1);
	for(i = 0; i < dy; i++)
		gy[i] = 255.0 * i/(dy-1);
	loadimage(col[Cgx], col[Cgx]->r, gx, dx);
	loadimage(col[Cgy], col[Cgy]->r, gy, dy);
	draw(screen, screen->r, col[Cred], nil, ZP);
	draw(screen, screen->r, col[Cgrn], col[Cgx], ZP);
	draw(screen, screen->r, col[Cblu], col[Cgy], ZP);
	flushimage(display, 1);
	sleep(5*SEC);
	exits(0);
}
