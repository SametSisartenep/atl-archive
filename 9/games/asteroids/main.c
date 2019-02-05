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
Channel *scrsync;
Vector O, Base;
Spacecraft ship;
Asteroid *asteroids;
int nasteroid;
Triangle shipmdl;
Point asteroidmdl[20];
Image *asteroidcol;
Image *provc, *retrovc;
double t0, Δt;

char stats[32];
int score;

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
vectopt(Vector v)
{
	return Pt(v.x, v.y);
}

Point
toworld(Point p)
{
	return addpt(vectopt(O), Pt(p.x*Base.x, p.y*Base.y));
}

void
wrapcoord(Vector *p)
{
	if(p->x > Dx(screen->r))
		p->x = 0;
	if(p->y > Dy(screen->r))
		p->y = 0;
	if(p->x < 0)
		p->x = Dx(screen->r);
	if(p->y < 0)
		p->y = Dy(screen->r);
}

void
drawstats(void)
{
	snprint(stats, sizeof stats, "SCORE %d", score);
	stringn(screen, toworld(Pt(10, Dy(screen->r)-10)), display->white, ZP, font, stats, strlen(stats));
	snprint(stats, sizeof stats, "SHIELDS %d", ship.shields);
	stringn(screen, toworld(Pt(10, Dy(screen->r)-10-1*font->height)), display->white, ZP, font, stats, strlen(stats));
}

void
redraw(void)
{
	int i, j;
	Triangle shipmdltrans;
	Point asteroidmdltrans[20];

	shipmdltrans = rotatriangle(shipmdl, ship.yaw*DEG, Pt(0, 0));

	draw(screen, screen->r, display->black, nil, ZP);
	triangle(screen, Trianpt(
		toworld(addpt(vectopt(ship.p), shipmdltrans.p0)),
		toworld(addpt(vectopt(ship.p), shipmdltrans.p1)),
		toworld(addpt(vectopt(ship.p), shipmdltrans.p2))
	), 0, display->white, ZP);
	line(screen, toworld(vectopt(ship.p)), toworld(vectopt(addvec(ship.p, ship.v))), 0, 0, 0, provc, ZP);
	line(screen, toworld(vectopt(ship.p)), toworld(vectopt(addvec(ship.p, mulvec(ship.v, -1)))), 0, 0, 0, retrovc, ZP);
	for(i = 0; i < nasteroid; i++){
		for(j = 0; j < nelem(asteroidmdl); j++)
			asteroidmdltrans[j] = toworld(addpt(vectopt(asteroids[i].p), asteroidmdl[j]));
		poly(screen, asteroidmdltrans, nelem(asteroidmdltrans), 0, 0, 0, asteroidcol, ZP);
	}
	drawstats();
	flushimage(display, 1);
}

void
gameover(void)
{
	char s[] = "GAME OVER";

	draw(screen, screen->r, display->black, nil, ZP);
	stringn(screen, toworld(Pt(Dx(screen->r)/2-strlen(s)/2*font->width, Dy(screen->r)/2)), retrovc, ZP, font, s, strlen(s));
	flushimage(display, 1);
	sleep(5*SEC);
}

void
mouse(void)
{
	return;
}

void
key(Rune r)
{
	Δt = (nsec()-t0)/1e9;

	switch(r){
	case Kdel:
	case 'q':
		threadexitsall(nil);
	case Kleft:
		ship.yaw += 20;
		break;
	case Kright:
		ship.yaw -= 20;
		break;
	case Kup:
		ship.v = addvec(ship.v, Vec(
			cos(ship.yaw*DEG) * THRUST*Δt,
			sin(ship.yaw*DEG) * THRUST*Δt));
		break;
	}
}

void
resized(void)
{
	if(getwindow(display, Refnone) < 0)
		fprint(2, "can't reattach to window\n");
	O = Vec(screen->r.min.x, screen->r.max.y);
	redraw();
}

void
refreshproc(void *)
{
	for(;;){
		send(scrsync, nil);
		sleep(SEC/FPS);
	}
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
	int i;
	double elev;

	ARGBEGIN{
	default: usage();
	}ARGEND;

	if(initdraw(nil, nil, "asteroids") < 0)
		sysfatal("initdraw: %r");
	mc = initmouse(nil, screen);
	if(mc == nil)
		sysfatal("initmouse: %r");
	kc = initkeyboard(nil);
	if(kc == nil)
		sysfatal("initkeyboard: %r");

	asteroidcol = allocimage(display, Rect(0, 0, 1, 1), screen->chan, 1, DRed);
	if(asteroidcol == nil)
		sysfatal("allocimage: %r");
	provc = allocimage(display, Rect(0, 0, 1, 1), screen->chan, 1, DGreen);
	if(provc == nil)
		sysfatal("allocimage: %r");
	retrovc = allocimage(display, Rect(0, 0, 1, 1), screen->chan, 1, DYellow);
	if(retrovc == nil)
		sysfatal("allocimage: %r");
	srand(time(0));
	O = Vec(screen->r.min.x, screen->r.max.y);
	Base = Vec(1, -1);

	elev = 0;
	for(i = 0; i < nelem(asteroidmdl)-1; i++){
		elev = frand()*12 + 10;
		asteroidmdl[i] = Pt(
			elev*cos(((double)i/nelem(asteroidmdl)) * 2*PI),
			elev*sin(((double)i/nelem(asteroidmdl)) * 2*PI)
		);
	}
	asteroidmdl[nelem(asteroidmdl)-1] = asteroidmdl[0];
	shipmdl = Trian(
		8, 0,
		-4, 4,
		-4, -4
	);

	ship.p = Vec(Dx(screen->r)/2, Dy(screen->r)/2);
	ship.yaw = 90;
	ship.shields = 5;
	nasteroid = 10;
	asteroids = emalloc(nasteroid*sizeof(Asteroid));
	for(i = 0; i < nasteroid; i++){
		srand(nsec());
		asteroids[i].p = Vec(
			ntruerand(Dx(screen->r)),
			ntruerand(Dy(screen->r))
		);
		asteroids[i].v = Vec(cos(frand())*50+10, sin(frand())*50+10);
	}
	scrsync = chancreate(1, 0);

	proccreate(refreshproc, nil, STACKSZ);

	t0 = nsec();

	for(;;){
		Alt a[] = {
			{mc->c, &mc->Mouse, CHANRCV},
			{mc->resizec, nil, CHANRCV},
			{kc->c, &r, CHANRCV},
			{scrsync, nil, CHANRCV},
			{nil, nil, CHANEND}
		};
		switch(alt(a)){
		case 0: mouse(); break;
		case 1: resized(); break;
		case 2: key(r); break;
		case 3: redraw(); break;
		}
		Δt = (nsec()-t0)/1e9;
		ship.p = addvec(ship.p, mulvec(ship.v, Δt));
		wrapcoord(&ship.p);
		for(i = 0; i < nasteroid; i++){
			asteroids[i].p = addvec(asteroids[i].p, mulvec(asteroids[i].v, Δt));
			wrapcoord(&asteroids[i].p);
			if(triangleXcircle(Trianpt(
				addpt(shipmdl.p0, vectopt(ship.p)),
				addpt(shipmdl.p1, vectopt(ship.p)),
				addpt(shipmdl.p2, vectopt(ship.p))), vectopt(asteroids[i].p), elev)){
				if(!asteroids[i].stillin)
					ship.shields--;
				if(ship.shields == 0)
					goto Gameover;
				asteroids[i].stillin = 1;
			}else
				asteroids[i].stillin = 0;
		}
		t0 += Δt*1e9;
	}
Gameover:
	gameover();
	threadexitsall(nil);
}
