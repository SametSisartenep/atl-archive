#include <u.h>
#include <libc.h>
#include <thread.h>
#include <draw.h>
#include <mouse.h>
#include <keyboard.h>
#include "dat.h"
#include "fns.h"

Mousectl *mctl;
Rune kdown[4];
Channel *scrsync;
Point orig;
Vector basis;
Spacecraft ship;
Asteroid *asteroids;
int nasteroid;
Triangle shipmdl, thrustermdl;
Point asteroidmdl[20];
Image *asteroidcol, *thrustercol;
Image *provc, *retrovc;
double t0, Δt;
int shownav, showthrust;

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
toscreen(Vector p)
{
	return addpt(orig, Pt(p.x*basis.x, p.y*basis.y));
}

Vector
fromscreen(Point p)
{
	return Vec(p.x-screen->r.min.x, screen->r.max.y-p.y);
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
	stringn(screen, addpt(screen->r.min, Pt(10, 10)), display->white, ZP, font, stats, strlen(stats));
	snprint(stats, sizeof stats, "SHIELDS %d", ship.shields);
	stringn(screen, addpt(screen->r.min, Pt(10, 10+font->height)), display->white, ZP, font, stats, strlen(stats));
}

void
drawship(void)
{
	Triangle shipmdltrans, thrustermdltrans;

	shipmdltrans = rotatriangle(shipmdl, ship.yaw*DEG, Pt(0, 0));
	triangle(screen, Trianpt(
		toscreen(addvec(ship.p, Vpt(shipmdltrans.p0))),
		toscreen(addvec(ship.p, Vpt(shipmdltrans.p1))),
		toscreen(addvec(ship.p, Vpt(shipmdltrans.p2)))
	), 0, display->white, ZP);
	if(showthrust){
		thrustermdltrans = rotatriangle(thrustermdl, ship.yaw*DEG, Pt(0, 0));
		filltriangle(screen, Trianpt(
			toscreen(addvec(ship.p, Vpt(thrustermdltrans.p0))),
			toscreen(addvec(ship.p, Vpt(thrustermdltrans.p1))),
			toscreen(addvec(ship.p, Vpt(thrustermdltrans.p2)))
		), thrustercol, ZP);
	}
}

void
drawasteroids(void)
{
	Point asteroidmdltrans[20];
	int i, j;

	for(i = 0; i < nasteroid; i++){
		for(j = 0; j < nelem(asteroidmdl); j++)
			asteroidmdltrans[j] = toscreen(addvec(asteroids[i].p, Vpt(asteroidmdl[j])));
		poly(screen, asteroidmdltrans, nelem(asteroidmdltrans), 0, 0, 0, asteroidcol, ZP);
	}
}

void
redraw(void)
{

	lockdisplay(display);
	draw(screen, screen->r, display->black, nil, ZP);
	if(shownav){
		line(screen, toscreen(ship.p), toscreen(addvec(ship.p, ship.v)), 0, 0, 0, provc, ZP);
		line(screen, toscreen(ship.p), toscreen(addvec(ship.p, mulvec(ship.v, -1))), 0, 0, 0, retrovc, ZP);
	}
	drawship();
	drawasteroids();
	drawstats();
	flushimage(display, 1);
	unlockdisplay(display);
}

void
gameover(void)
{
	char s[] = "GAME OVER";

	lockdisplay(display);
	draw(screen, screen->r, display->black, nil, ZP);
	stringn(screen, toscreen(Vec(Dx(screen->r)/2-strlen(s)/2*font->width, Dy(screen->r)/2)), retrovc, ZP, font, s, strlen(s));
	flushimage(display, 1);
	unlockdisplay(display);
	sleep(5*SEC);
}

void
scrsynproc(void *)
{
	for(;;){
		send(scrsync, nil);
		sleep(SEC/FPS);
	}
}

void
kbdproc(void *)
{
	Rune r;
	char buf[128], *s;
	int fd, n, i;

	threadsetname("kbdproc");
	if((fd = open("/dev/kbd", OREAD)) < 0)
		sysfatal("open: %r");
	for(;;){
		if((n = read(fd, buf, sizeof(buf)-1)) <= 0)
			sysfatal("read: %r");
		buf[n] = 0;
		for(s = buf; s-buf < n;){
			switch(*s++){
			case 'k':
			case 'K':
				for(i = 0; *s && i < nelem(kdown)-1; i++){
					s += chartorune(&r, s);
					if(r == 'n')
						shownav = !shownav;
					kdown[i] = r;
				}
				kdown[i] = 0;
				break;
			default:
				while(*s++)
					;
			}
		}
	}
}

void
mouse(void)
{
	Vector p;

	Δt = (nsec()-t0)/1e9;
	if((mctl->buttons & 1) != 0){
		p = subvec(fromscreen(mctl->xy), ship.p);
		ship.yaw = atan2(p.y, p.x)*RAD;
		ship.v = addvec(ship.v, Vec(
			cos(ship.yaw*DEG) * THRUST*Δt,
			sin(ship.yaw*DEG) * THRUST*Δt));
	}
}

void
resized(void)
{
	lockdisplay(display);
	if(getwindow(display, Refnone) < 0)
		fprint(2, "can't reattach to window\n");
	orig = Pt(screen->r.min.x, screen->r.max.y);
	unlockdisplay(display);
	redraw();
}

void
handlekeys(void)
{
	int i;

	showthrust = 0;
	for(i = 0; kdown[i]; i++)
		switch(kdown[i]){
		case Kdel:
		case 'q':
			threadexitsall(nil);
		case Kleft:
			ship.yaw += 5;
			break;
		case Kright:
			ship.yaw -= 5;
			break;
		case Kup:
			showthrust = 1;
			ship.v = addvec(ship.v, Vec(
				cos(ship.yaw*DEG) * THRUST*Δt,
				sin(ship.yaw*DEG) * THRUST*Δt));
			break;
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
	double elev, avgelev;
	int i;

	ARGBEGIN{
	default: usage();
	}ARGEND;

	if(initdraw(nil, nil, "asteroids") < 0)
		sysfatal("initdraw: %r");
	if((mctl = initmouse(nil, screen)) == nil)
		sysfatal("initmouse: %r");
	asteroidcol = allocimage(display, Rect(0, 0, 1, 1), screen->chan, 1, DRed);
	if(asteroidcol == nil)
		sysfatal("allocimage: %r");
	thrustercol = allocimage(display, Rect(0, 0, 1, 1), screen->chan, 1, DYellow);
	if(thrustercol == nil)
		sysfatal("allocimage: %r");
	provc = allocimage(display, Rect(0, 0, 1, 1), screen->chan, 1, DGreen);
	if(provc == nil)
		sysfatal("allocimage: %r");
	retrovc = allocimage(display, Rect(0, 0, 1, 1), screen->chan, 1, DYellow);
	if(retrovc == nil)
		sysfatal("allocimage: %r");
	srand(time(0));
	orig = Pt(screen->r.min.x, screen->r.max.y);
	basis = Vec(1, -1);
	avgelev = 0;
	for(i = 0; i < nelem(asteroidmdl)-1; i++){
		elev = frand()*12 + 10;
		asteroidmdl[i] = Pt(
			elev*cos(((double)i/nelem(asteroidmdl)) * 2*PI),
			elev*sin(((double)i/nelem(asteroidmdl)) * 2*PI)
		);
		avgelev += elev;
	}
	asteroidmdl[nelem(asteroidmdl)-1] = asteroidmdl[0];
	avgelev /= i;
	shipmdl = Trian(
		8, 0,
		-4, 4,
		-4, -4
	);
	thrustermdl = Trian(
		-12, 0,
		-4, 2,
		-4, -2
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
	display->locking = 1;
	unlockdisplay(display);
	scrsync = chancreate(1, 0);
	proccreate(scrsynproc, nil, STACKSZ);
	proccreate(kbdproc, nil, STACKSZ);
	t0 = nsec();
	for(;;){
		enum{MOUSE, RESIZE, SCRSYN};
		Alt a[] = {
			{mctl->c, &mctl->Mouse, CHANRCV},
			{mctl->resizec, nil, CHANRCV},
			{scrsync, nil, CHANRCV},
			{nil, nil, CHANEND}
		};
		switch(alt(a)){
		case MOUSE: mouse(); break;
		case RESIZE: resized(); break;
		case SCRSYN: redraw(); break;
		}
		Δt = (nsec()-t0)/1e9;
		handlekeys();
		ship.p = addvec(ship.p, mulvec(ship.v, Δt));
		wrapcoord(&ship.p);
		for(i = 0; i < nasteroid; i++){
			asteroids[i].p = addvec(asteroids[i].p, mulvec(asteroids[i].v, Δt));
			wrapcoord(&asteroids[i].p);
			if(triangleXcircle(Trianpt(
				addpt(shipmdl.p0, vectopt(ship.p)),
				addpt(shipmdl.p1, vectopt(ship.p)),
				addpt(shipmdl.p2, vectopt(ship.p))
			   ), vectopt(asteroids[i].p), avgelev)){
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
