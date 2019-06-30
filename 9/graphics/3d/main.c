#include <u.h>
#include <libc.h>
#include <thread.h>
#include <draw.h>
#include <mouse.h>
#include <keyboard.h>
#include "geometry.h"
#include "graphics.h"
#include "dat.h"
#include "fns.h"

Mousectl *mctl;
Keyboardctl *kctl;
Point orig;
Point2 basis;
double t0, Δt;
double θ, ω;
RWLock worldlock;
Triangle3 tri;

Camera cam0, cam1, cam2, cam3, *maincam;
int camno;

#pragma varargck type "v" Point2
int
vfmt(Fmt *f)
{
	Point2 p;

	p = va_arg(f->args, Point2);
	return fmtprint(f, "[%g %g %g]", p.x, p.y, p.w);
}

#pragma varargck type "V" Point3
int
Vfmt(Fmt *f)
{
	Point3 p;

	p = va_arg(f->args, Point3);
	return fmtprint(f, "[%g %g %g %g]", p.x, p.y, p.z, p.w);
}

Point2
flatten(Point3 p)
{
	Matrix3 S = {
		Dx(screen->r)/2, 0, 0, 0,
		0, Dy(screen->r)/2, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1,
	}, T = {
		1, 0, 0, 1,
		0, 1, 0, 1,
		0, 0, 1, 0,
		0, 0, 0, 1,
	};

	if(p.w != 0)
		p = divpt3(p, p.w);
	mulm3(S, T);
	p = xform3(p, S);
	return (Point2){p.x, p.y, p.w};
}

Point
toscreen(Point2 p)
{
	return addpt(orig, Pt(p.x*basis.x, p.y*basis.y));
}

Point2
fromscreen(Point p)
{
	return Pt2(p.x-screen->r.min.x, screen->r.max.y-p.y, 1);
}

void
drawstats(void)
{
	int i;

	snprint(stats[Sfov], sizeof(stats[Sfov]), "FOV %g°", maincam->fov);
	snprint(stats[Scamno], sizeof(stats[Scamno]), "CAM %d", camno);
	snprint(stats[Scampos], sizeof(stats[Scampos]), "%V", maincam->p);
	snprint(stats[Syaw], sizeof(stats[Syaw]), "%V", maincam->zb);
	snprint(stats[Sp0vcs], sizeof(stats[Sp0vcs]), "VCS %V", rframexform(tri.p0, *maincam));
	snprint(stats[Sp0norm], sizeof(stats[Sp0norm]), "NOR %V", xform3(rframexform(tri.p0, *maincam), maincam->proj));
	snprint(stats[Sp0view], sizeof(stats[Sp0view]), "VIE %v %v %v", flatten(xform3(rframexform(tri.p0, *maincam), maincam->proj)), flatten(xform3(rframexform(tri.p1, *maincam), maincam->proj)), flatten(xform3(rframexform(tri.p2, *maincam), maincam->proj)));
	snprint(stats[Sp0scr], sizeof(stats[Sp0scr]), "SCR %P %P %P", toscreen(flatten(xform3(rframexform(tri.p0, *maincam), maincam->proj))), toscreen(flatten(xform3(rframexform(tri.p1, *maincam), maincam->proj))), toscreen(flatten(xform3(rframexform(tri.p2, *maincam), maincam->proj))));
	for(i = 0; i < Se; i++)
		stringn(screen, addpt(screen->r.min, Pt(10, 10 + i*font->height)), display->white, ZP, font, stats[i], sizeof(stats[i]));
}

void
redraw(void)
{
	Triangle trit;

	trit.p0 = toscreen(flatten(xform3(rframexform(tri.p0, *maincam), maincam->proj)));
	trit.p1 = toscreen(flatten(xform3(rframexform(tri.p1, *maincam), maincam->proj)));
	trit.p2 = toscreen(flatten(xform3(rframexform(tri.p2, *maincam), maincam->proj)));
	lockdisplay(display);
	draw(maincam->viewport, maincam->viewport->r, display->black, nil, ZP);
	filltriangle(maincam->viewport, trit, display->white, ZP);
	drawstats();
	flushimage(display, 1);
	unlockdisplay(display);
}

void
scrsynproc(void *)
{
	threadsetname("scrsynproc");
	for(;;){
		rlock(&worldlock);
		redraw();
		runlock(&worldlock);
		sleep(SEC/FPS);
	}
}

void
screenshot(void)
{
	int fd;
	static char buf[128];

	enter("Path", buf, sizeof buf, mctl, kctl, nil);
	if(buf[0] == 0)
		return;
	fd = create(buf, OWRITE, 0644);
	if(fd < 0)
		sysfatal("open: %r");
	if(writeimage(fd, screen, 1) < 0)
		sysfatal("writeimage: %r");
	close(fd);
}

void
mouse(void)
{
	Matrix3 RL = {
		cos(5*DEG), sin(5*DEG), 0, 0,
		-sin(5*DEG), cos(5*DEG), 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1,
	}, RR = {
		cos(-5*DEG), sin(-5*DEG), 0, 0,
		-sin(-5*DEG), cos(-5*DEG), 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1,
	};

	if(mctl->buttons & 1)
		placecamera(maincam, maincam->p, maincam->zb, xform3(maincam->yb, RL));
	if(mctl->buttons & 4)
		placecamera(maincam, maincam->p, maincam->zb, xform3(maincam->yb, RR));
	if(mctl->buttons & 8){
		maincam->fov -= 5;
		if(maincam->fov < 1)
			maincam->fov = 1;
		reloadcamera(maincam);
	}
	if(mctl->buttons & 16){
		maincam->fov += 5;
		if(maincam->fov > 359)
			maincam->fov = 359;
		reloadcamera(maincam);
	}
}

void
key(Rune r)
{
	Matrix3 TL = {
		cos(5*DEG), 0, sin(5*DEG), 0,
		0, 1, 0, 0,
		-sin(5*DEG), 0, cos(5*DEG), 0,
		0, 0, 0, 1,
	}, TR = {
		cos(-5*DEG), 0, sin(-5*DEG), 0,
		0, 1, 0, 0,
		-sin(-5*DEG), 0, cos(-5*DEG), 0,
		0, 0, 0, 1,
	};

	switch(r){
	case Kdel:
	case 'q':
		threadexitsall(nil);
	case Kup:
		placecamera(maincam, addpt3(maincam->p, mulpt3(maincam->zb, 0.2)), maincam->zb, maincam->yb);
		break;
	case Kdown:
		placecamera(maincam, subpt3(maincam->p, mulpt3(maincam->zb, 0.2)), maincam->zb, maincam->yb);
		break;
	case Kleft:
		aimcamera(maincam, xform3(maincam->zb, TL));
		break;
	case Kright:
		aimcamera(maincam, xform3(maincam->zb, TR));
		break;
	case KF|1:
		maincam = &cam0;
		camno = 1;
		break;
	case KF|2:
		maincam = &cam1;
		camno = 2;
		break;
	case KF|3:
		maincam = &cam2;
		camno = 3;
		break;
	case KF|4:
		maincam = &cam3;
		camno = 4;
		break;
	case KF|12:
		screenshot();
		break;
	}
}

void
resize(void)
{
	lockdisplay(display);
	if(getwindow(display, Refnone) < 0)
		fprint(2, "can't reattach to window\n");
	unlockdisplay(display);
	orig = Pt(screen->r.min.x, screen->r.max.y);
	maincam->viewport = screen;
	reloadcamera(maincam);
}

void
usage(void)
{
	fprint(2, "usage: %s\n", argv0);
	exits("usage");
}

void
threadmain(int argc, char *argv[])
{
	Rune r;

	fmtinstall('V', Vfmt);
	fmtinstall('v', vfmt);
	ARGBEGIN{
	default: usage();
	}ARGEND;
	if(argc != 0)
		usage();
	if(initdraw(nil, nil, "3d") < 0)
		sysfatal("initdraw: %r");
	if((mctl = initmouse(nil, screen)) == nil)
		sysfatal("initmouse: %r");
	if((kctl = initkeyboard(nil)) == nil)
		sysfatal("initkeyboard: %r");
	orig = Pt(screen->r.min.x, screen->r.max.y);
	basis = Vec2(1, -1);
	tri.p0 = Pt3(0, 1, 2, 1);
	tri.p1 = Pt3(-1, 0, 2, 1);
	tri.p2 = Pt3(1, 0, 2, 1);
	placecamera(&cam0, Pt3(2, 0, 4, 1), Pt3(0, 0, 0, 1), Vec3(0, 1, 0));
	configcamera(&cam0, screen, 90, 0.1, 100, Ppersp);
	placecamera(&cam1, Pt3(-2, 0, 4, 1), Pt3(0, 0, 0, 1), Vec3(0, 1, 0));
	configcamera(&cam1, screen, 90, 0.1, 100, Ppersp);
	placecamera(&cam2, Pt3(-2, 0, -4, 1), Pt3(0, 0, 0, 1), Vec3(0, 1, 0));
	configcamera(&cam2, screen, 90, 0.1, 100, Ppersp);
	placecamera(&cam3, Pt3(2, 0, -4, 1), Pt3(0, 0, 0, 1), Vec3(0, 1, 0));
	configcamera(&cam3, screen, 90, 0.1, 100, Ppersp);
	maincam = &cam0;
	camno = 1;
	display->locking = 1;
	unlockdisplay(display);
	proccreate(scrsynproc, nil, STACK);
	ω = 1;
	t0 = nsec();
	for(;;){
		enum {MOUSE, KBD, RESIZE};
		Alt a[] = {
			{mctl->c, &mctl->Mouse, CHANRCV},
			{kctl->c, &r, CHANRCV},
			{mctl->resizec, nil, CHANRCV},
			{nil, nil, CHANNOBLK}
		};
		wlock(&worldlock);
		switch(alt(a)){
		case MOUSE: mouse(); break;
		case KBD: key(r); break;
		case RESIZE: resize(); break;
		}
		Δt = (nsec()-t0)/1e9;
		θ = ω*Δt;
		t0 += Δt*1e9;
		Matrix3 R = {
			cos(θ/3), 0, sin(θ/3), 0,
			0, 1, 0, 0,
			-sin(θ/3), 0, cos(θ/3), 0,
			0, 0, 0, 1,
		}, T1 = {
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, -tri.p0.z,
			0, 0, 0, 1,
		}, T2 = {
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, tri.p0.z,
			0, 0, 0, 1,
		};
		mulm3(R, T1);
		mulm3(T2, R);
		tri.p0 = xform3(tri.p0, T2);
		tri.p1 = xform3(tri.p1, T2);
		tri.p2 = xform3(tri.p2, T2);
		wunlock(&worldlock);
		sleep(16);
	}
}
