#include <u.h>
#include <libc.h>
#include <draw.h>

enum { SEC = 1000 };

enum {
	Cred,
	Cgrn,

	Cbg,

	Cgx,
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
	fprint(2, "usage: spranimate\n");
	exits("usage");
}

void
main(int argc, char *argv[])
{
	Image *spnr, *spnmsk;
	double θ;
	uchar *gx;
	int ds, i, x, y, inc, frame;

	ARGBEGIN{
	default: usage();
	}ARGEND;

	if(initdraw(nil, nil, "spranimate") < 0)
		sysfatal("initdraw: %r");
	dx = Dx(screen->r), dy = Dy(screen->r);
	col[Cred] = eallocimage(display, Rect(0, 0, 1, 1), RGB24, 1, DRed);
	col[Cgrn] = eallocimage(display, Rect(0, 0, 1, 1), RGB24, 1, DGreen);
	col[Cbg] = eallocimage(display, Rect(0, 0, 1, 1), RGB24, 1, DNofill);
	ds = dx > dy ? dy : dx;
	spnr = eallocimage(display, Rect(0, 0, ds*60, ds), RGBA32, 0, DTransparent);
	spnmsk = eallocimage(display, Rect(0, 0, ds, ds), GREY1, 0, DWhite);
	col[Cgx] = eallocimage(display, Rect(0, 0, 60, 1), GREY8, 1, DNofill);
	gx = emalloc(60);
	for(i = 0; i < 60; i++)
		gx[i] = 255.0 * i/(60-1);
	for(i = 0; i < 60; i++){
		θ = 2*PI * i/60;
		x = (ds/2 - 5) * cos(θ);
		y = (ds/2 - 5) * sin(θ);
		line(spnr, Pt(x + ds/2 + ds*i, y + ds/2), Pt(-x + ds/2 + ds*i, -y + ds/2), Endarrow, Endsquare, 1, display->black, ZP);
	}
	loadimage(col[Cgx], col[Cgx]->r, gx, dx);
	x = inc = frame = 0;
	for(;;){
		draw(col[Cbg], col[Cbg]->r, col[Cred], nil, ZP);
		if(x == 0)
			inc = 1;
		else if(x == 60-1)
			inc = -1;
		x += inc;
		gendraw(col[Cbg], col[Cbg]->r, col[Cgrn], ZP, col[Cgx], Pt(x, 0));
		draw(screen, screen->r, col[Cbg], nil, ZP);
		frame = (frame+1) % 60;
		gendraw(screen, screen->r, spnr, Pt(frame*ds, 0), spnmsk, ZP);
		flushimage(display, 1);
		sleep(SEC/60);
	}
}
