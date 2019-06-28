#include <u.h>
#include <libc.h>
#include <thread.h>
#include <draw.h>
#include <mouse.h>
#include <keyboard.h>
#include "dat.h"
#include "fns.h"

Mousectl *mc;
Keyboardctl *kc;
Image *color;
Point O, Base;
Triangle tr, rt, irt;
int α;
char αstat[64];

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

Point
tobase(Point p)
{
	return addpt(O, Pt(p.x*Base.x, p.y*Base.y));
}

void
redraw(void)
{
	draw(screen, screen->r, display->black, nil, ZP);
	filltriangle(screen, tr, display->white, ZP);
	triangle(screen, rt, color, ZP);
	triangle(screen, irt, color, ZP);
	stringn(screen, addpt(screen->r.min, Pt(20, 10)), display->white, ZP, font, αstat, sizeof αstat-1);
	flushimage(display, 1);
}

void
mouse(void)
{
	return;
}

void
key(Rune r)
{
	switch(r){
	case Kdel:
	case 'q':
		threadexitsall(nil);
	case Kup:
		α++;
		snprint(αstat, sizeof αstat-1, "α: %d", α);
		break;
	case Kdown:
		α--;
		snprint(αstat, sizeof αstat-1, "α: %d", α);
		break;
	case ' ':
		tr = gentriangle();
		rt = rotatriangle(tr, α*DEG);
		irt = inrotatriangle(tr, α*DEG);
		break;
	}
}

void
resized(void)
{
	if(getwindow(display, Refnone) < 0)
		fprint(2, "can't reattach to window\n");
	O = Pt(screen->r.min.x, screen->r.max.y);
	redraw();
}

void
usage(void)
{
	fprint(2, "usage: %s\n", argv0);
	threadexitsall("usage");
}

void
threadmain(int argc, char *argv[])
{
	Rune r;
	char *s;

	ARGBEGIN{
	case 'a':
		α = strtol(EARGF(usage()), &s, 0);
		if(*s != 0)
			usage();
		break;
	default: usage();
	}ARGEND;

	if(initdraw(nil, nil, nil) < 0)
		sysfatal("initdraw: %r");
	mc = initmouse(nil, screen);
	if(mc == nil)
		sysfatal("initmouse: %r");
	kc = initkeyboard(nil);
	if(kc == nil)
		sysfatal("initkeyboard: %r");

	color = allocimage(display, Rect(0, 0, 1, 1), screen->chan, 1, DRed);
	if(color == nil)
		sysfatal("allocimage: %r");
	O = Pt(screen->r.min.x, screen->r.max.y);
	Base = Pt(1, -1);
	tr = gentriangle();
	rt = rotatriangle(tr, α*DEG);
	irt = inrotatriangle(tr, α*DEG);
	snprint(αstat, sizeof αstat-1, "α: %d", α);

	for(;;){
		Alt a[] = {
			{mc->c, &mc->Mouse, CHANRCV},
			{kc->c, &r, CHANRCV},
			{mc->resizec, nil, CHANRCV},
			{nil, nil, CHANNOBLK}
		};

		switch(alt(a)){
		case 0: mouse(); break;
		case 1: key(r); break;
		case 2: resized(); break;
		}

		redraw();
	}
}
