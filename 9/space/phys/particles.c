#include <u.h>
#include <libc.h>
#include <thread.h>
#include <draw.h>
#include <mouse.h>
#include <keyboard.h>
#include "dat.h"
#include "fns.h"

int steps, paused;
Vec Base, O;
vlong t0, t;
Mousectl *mc;
Keyboardctl *kc;
Particle *gas;
int npart;
Image *statc;
Font *statf;

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
initgas(void)
{
	Particle *p;

	statc = allocimage(display, Rect(0, 0, 1, 1), screen->chan, 1, DBlue);
	statf = openfont(display, "/lib/font/bit/dejavusans/unicode.12.font");
	gas = emalloc(npart*sizeof(Particle));
	Base = (Vec){PIX2M, -PIX2M};
	O = (Vec){screen->r.min.x, screen->r.max.y};

	for(p = gas; (p-gas) < npart; p++){
		p->p = (Vec){
			nrand(Dx(screen->r))/PIX2M,
			nrand(Dy(screen->r))/PIX2M
		};
		p->v = (Vec){0, 0};
		p->m = frand()*1000;
	}
}

void
paintgas(void)
{
	char buf[1024];
	Point pt;
	Particle *p;

	for(p = gas; (p-gas) < npart; p++){
		pt.x = O.x + p->p.x*Base.x;
		pt.y = O.y + p->p.y*Base.y;
		fillellipse(screen, pt, 2, 2, display->white, ZP);
		snprint(buf, sizeof buf, "(%g,%g)", p->p.x, p->p.y);
		string(screen, subpt(pt, Pt(0, 2+font->height)), statc, ZP, statf, buf);
	}
}

void
applyforces(void)
{
	Vec a;
	double Δt;
	Particle *p;

	a = (Vec){0, Grav};
	t = nsec();
	Δt = (t-t0)/1e9*steps;
	t0 = t;

	for(p = gas; (p-gas) < npart; p++)
			if(p->m != 0){
				p->v = addvec(p->v, mulvec(a, Δt));
				p->p = addvec(p->p, mulvec(p->v, Δt));
				if(p->p.y < 0+2/PIX2M)
					p->p.y = Dy(screen->r)/PIX2M-2/PIX2M;
			}
}

void
redraw(void)
{
	draw(screen, screen->r, display->black, nil, ZP);
	paintgas();
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
		threadexitsall(nil);
	case ' ':
		paused ^= 1;
		t0 = nsec();
		break;
	}
}

void
gotresized(void)
{
	if(getwindow(display, Refnone) < 0)
		fprint(2, "can't reattach to window\n");
	redraw();
}

void
usage(void)
{
	fprint(2, "usage: %s [-s steps] [-n nparticles]\n", argv0);
	threadexitsall("usage");
}

void
threadmain(int argc, char *argv[])
{
	Rune r;
	char *s;

	ARGBEGIN{
	case 's':
		steps = strtol(EARGF(usage()), &s, 0);
		if(*s != 0)
			usage();
		break;
	case 'n':
		npart = strtol(EARGF(usage()), &s, 0);
		if(*s != 0)
			usage();
		break;
	default: usage();
	}ARGEND;

	if(initdraw(nil, nil, "particles") < 0)
		sysfatal("initdraw: %r");
	mc = initmouse(nil, screen);
	if(mc == nil)
		sysfatal("initmouse: %r");
	kc = initkeyboard(nil);
	if(kc == nil)
		sysfatal("initkeyboard: %r");

	if(steps < 1)
		steps = 4;
	if(npart < 1)
		npart = 20;
	t0 = nsec();
	initgas();

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
		case 2: gotresized(); break;
		}

		if(!paused){
			redraw();
			applyforces();
		}
	}
}
