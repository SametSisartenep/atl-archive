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

Rune keys[Ke] = {
 [K↑]		= Kup,
 [K↓]		= Kdown,
 [K←]		= Kleft,
 [K→]		= Kright,
 [Krise]	= Kpgup,
 [Kfall]	= Kpgdown,
 [KR↑]		= 'w',
 [KR↓]		= 's',
 [KR←]		= 'a',
 [KR→]		= 'd',
 [KR↺]		= 'q',
 [KR↻]		= 'e',
 [Kcam0]	= KF|1,
 [Kcam1]	= KF|2,
 [Kcam2]	= KF|3,
 [Kcam3]	= KF|4,
 [Kscrshot]	= KF|12
};

Mousectl *mctl;
Keyboardctl *kctl;
int kdown;
ReferenceFrame scrrf;
double t0, Δt;
double θ, ω;
RWLock worldlock;
Mesh model;
char *mdlpath = "../threedee/mdl/rocket.obj";

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

void *
emalloc(ulong n)
{
	void *p;

	p = malloc(n);
	if(p == nil)
		sysfatal("malloc: %r");
	setmalloctag(p, getcallerpc(&n));
	return p;
}

void *
erealloc(void *ptr, ulong n)
{
	void *p;

	p = realloc(ptr, n);
	if(p == nil)
		sysfatal("realloc: %r");
	setrealloctag(p, getcallerpc(&ptr));
	return p;
}

int
depthcmp(void *a, void *b)
{
	Triangle3 *ta, *tb;
	double za, zb;

	ta = (Triangle3 *)a;
	tb = (Triangle3 *)b;
	za = (ta->p0.z + ta->p1.z + ta->p2.z)/3;
	zb = (tb->p0.z + tb->p1.z + tb->p2.z)/3;
	return zb-za;
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
	p = rframexform(p, scrrf);
	return addpt(Pt(screen->r.min.x, screen->r.max.y), (Point){p.x, p.y});
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

	snprint(stats[Scamno], sizeof(stats[Scamno]), "CAM %d", camno);
	snprint(stats[Scampos], sizeof(stats[Scampos]), "%V", maincam->p);
	snprint(stats[Sfov], sizeof(stats[Sfov]), "FOV %g°", maincam->fov);
	for(i = 0; i < Se; i++)
		stringn(screen, addpt(screen->r.min, Pt(10, 10 + i*font->height)), display->black, ZP, font, stats[i], sizeof(stats[i]));
}

void
redraw(void)
{
	Triangle3 tmp;
	TTriangle3 *vistris;
	Triangle trit;
	Point3 n;
	u8int c;
	int i, nvistri;

	vistris = nil;
	nvistri = 0;
	for(i = 0; i < model.ntri; i++){
		/* world to camera */
		tmp.p0 = rframexform3(model.tris[i].p0, *maincam);
		tmp.p1 = rframexform3(model.tris[i].p1, *maincam);
		tmp.p2 = rframexform3(model.tris[i].p2, *maincam);
		n = normvec3(crossvec3(subpt3(tmp.p1, tmp.p0), subpt3(tmp.p2, tmp.p0)));
		if(dotvec3(n, mulpt3(tmp.p0, -1)) <= 0)
			continue;
		/* camera to projected ndc */
		tmp.p0 = xform3(tmp.p0, maincam->proj);
		tmp.p1 = xform3(tmp.p1, maincam->proj);
		tmp.p2 = xform3(tmp.p2, maincam->proj);
		if(tmp.p0.x > tmp.p0.w || tmp.p0.x < -tmp.p0.w ||
		   tmp.p0.y > tmp.p0.w || tmp.p0.y < -tmp.p0.w ||
		   tmp.p0.z > tmp.p0.w || tmp.p0.z < 0)
			continue;
		vistris = erealloc(vistris, ++nvistri*sizeof(TTriangle3));
		vistris[nvistri-1] = (TTriangle3)tmp;
		c = 0xff*fabs(dotvec3(n, Vec3(0, 0, 1)));
		vistris[nvistri-1].tx = allocimage(display, Rect(0, 0, 1, 1), screen->chan, 1, c<<24|c<<16|c<<8|0xff);
	}
	qsort(vistris, nvistri, sizeof(TTriangle3), depthcmp);
	lockdisplay(display);
	draw(maincam->viewport, maincam->viewport->r, display->white, nil, ZP);
	for(i = 0; i < nvistri; i++){
		/* ndc to screen */
		trit.p0 = toscreen(flatten(vistris[i].p0));
		trit.p1 = toscreen(flatten(vistris[i].p1));
		trit.p2 = toscreen(flatten(vistris[i].p2));
		filltriangle(maincam->viewport, trit, vistris[i].tx, ZP);
		triangle(maincam->viewport, trit, 0, display->black, ZP);
		freeimage(vistris[i].tx);
	}
	drawstats();
	flushimage(display, 1);
	unlockdisplay(display);
	free(vistris);
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
kbdproc(void *)
{
	Rune r, *a;
	char buf[128], *s;
	int fd, n;

	threadsetname("kbdproc");
	if((fd = open("/dev/kbd", OREAD)) < 0)
		sysfatal("kbdproc: %r");
	memset(buf, 0, sizeof buf);
	for(;;){
		if(buf[0] != 0){
			n = strlen(buf)+1;
			memmove(buf, buf+n, sizeof(buf)-n);
		}
		if(buf[0] == 0){
			if((n = read(fd, buf, sizeof(buf)-1)) <= 0)
				break;
			buf[n-1] = 0;
			buf[n] = 0;
		}
		if(buf[0] == 'c'){
			if(utfrune(buf, Kdel)){
				close(fd);
				threadexitsall(nil);
			}
		}
		if(buf[0] != 'k' && buf[0] != 'K')
			continue;
		s = buf+1;
		kdown = 0;
		while(*s){
			s += chartorune(&r, s);
			for(a = keys; a < keys+Ke; a++)
				if(r == *a){
					kdown |= 1 << a-keys;
					break;
				}
		}
	}
}

void
handlekeys(void)
{
	Quaternion q, qtmp;

	if(kdown & 1<<K↑)
		placecamera(maincam, subpt3(maincam->p, maincam->zb), maincam->zb, maincam->yb);
	if(kdown & 1<<K↓)
		placecamera(maincam, addpt3(maincam->p, maincam->zb), maincam->zb, maincam->yb);
	if(kdown & 1<<K←)
		placecamera(maincam, subpt3(maincam->p, maincam->xb), maincam->zb, maincam->yb);
	if(kdown & 1<<K→)
		placecamera(maincam, addpt3(maincam->p, maincam->xb), maincam->zb, maincam->yb);
	if(kdown & 1<<Krise)
		placecamera(maincam, addpt3(maincam->p, maincam->yb), maincam->zb, maincam->yb);
	if(kdown & 1<<Kfall)
		placecamera(maincam, subpt3(maincam->p, maincam->yb), maincam->zb, maincam->yb);
	if(kdown & 1<<KR↑){
		q = Quatvec(cos(5*DEG/2), mulpt3(maincam->xb, sin(5*DEG/2)));
		qtmp = Quatvec(0, maincam->zb);
		qtmp = mulq(mulq(q, qtmp), invq(q));
		aimcamera(maincam, Vec3(qtmp.i, qtmp.j, qtmp.k));
	}
	if(kdown & 1<<KR↓){
		q = Quatvec(cos(-5*DEG/2), mulpt3(maincam->xb, sin(-5*DEG/2)));
		qtmp = Quatvec(0, maincam->zb);
		qtmp = mulq(mulq(q, qtmp), invq(q));
		aimcamera(maincam, Vec3(qtmp.i, qtmp.j, qtmp.k));
	}
	if(kdown & 1<<KR←){
		q = Quatvec(cos(5*DEG/2), mulpt3(maincam->yb, sin(5*DEG/2)));
		qtmp = Quatvec(0, maincam->zb);
		qtmp = mulq(mulq(q, qtmp), invq(q));
		aimcamera(maincam, Vec3(qtmp.i, qtmp.j, qtmp.k));
	}
	if(kdown & 1<<KR→){
		q = Quatvec(cos(-5*DEG/2), mulpt3(maincam->yb, sin(-5*DEG/2)));
		qtmp = Quatvec(0, maincam->zb);
		qtmp = mulq(mulq(q, qtmp), invq(q));
		aimcamera(maincam, Vec3(qtmp.i, qtmp.j, qtmp.k));
	}
	if(kdown & 1<<KR↺){
		q = Quatvec(cos(5*DEG/2), mulpt3(maincam->zb, sin(5*DEG/2)));
		qtmp = Quatvec(0, maincam->yb);
		qtmp = mulq(mulq(q, qtmp), invq(q));
		placecamera(maincam, maincam->p, maincam->zb, Vec3(qtmp.i, qtmp.j, qtmp.k));
	}
	if(kdown & 1<<KR↻){
		q = Quatvec(cos(-5*DEG/2), mulpt3(maincam->zb, sin(-5*DEG/2)));
		qtmp = Quatvec(0, maincam->yb);
		qtmp = mulq(mulq(q, qtmp), invq(q));
		placecamera(maincam, maincam->p, maincam->zb, Vec3(qtmp.i, qtmp.j, qtmp.k));
	}
	if(kdown & 1<<Kcam0){
		maincam = &cam0;
		camno = 1;
	}
	if(kdown & 1<<Kcam1){
		maincam = &cam1;
		camno = 2;
	}
	if(kdown & 1<<Kcam2){
		maincam = &cam2;
		camno = 3;
	}
	if(kdown & 1<<Kcam3){
		maincam = &cam3;
		camno = 4;
	}
	if(kdown & 1<<Kscrshot)
		screenshot();
}

void
resize(void)
{
	lockdisplay(display);
	if(getwindow(display, Refnone) < 0)
		fprint(2, "can't reattach to window\n");
	unlockdisplay(display);
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
	fmtinstall('V', Vfmt);
	fmtinstall('v', vfmt);
	ARGBEGIN{
	default: usage();
	case 'l':
		mdlpath = EARGF(usage());
		break;
	}ARGEND;
	if(argc != 0)
		usage();
	if(initdraw(nil, nil, "3d") < 0)
		sysfatal("initdraw: %r");
	if((mctl = initmouse(nil, screen)) == nil)
		sysfatal("initmouse: %r");
	scrrf.p = Pt2(0, 0, 1);
	scrrf.xb = Vec2(1, 0);
	scrrf.yb = Vec2(0, -1);
	placecamera(&cam0, Pt3(2, 0, -4, 1), Pt3(0, 0, 0, 1), Vec3(0, 1, 0));
	configcamera(&cam0, screen, 90, 0.1, 100, Ppersp);
	placecamera(&cam1, Pt3(-2, 0, -4, 1), Pt3(0, 0, 0, 1), Vec3(0, 1, 0));
	configcamera(&cam1, screen, 90, 0.1, 100, Ppersp);
	placecamera(&cam2, Pt3(-2, 0, 4, 1), Pt3(0, 0, 0, 1), Vec3(0, 1, 0));
	configcamera(&cam2, screen, 90, 0.1, 100, Ppersp);
	placecamera(&cam3, Pt3(2, 0, 4, 1), Pt3(0, 0, 0, 1), Vec3(0, 1, 0));
	configcamera(&cam3, screen, 90, 0.1, 100, Ppersp);
	maincam = &cam0;
	camno = 1;
	if((model.ntri = objread(mdlpath, &model.tris)) < 0)
		sysfatal("objread: %r");
	display->locking = 1;
	unlockdisplay(display);
	proccreate(scrsynproc, nil, STACK);
	proccreate(kbdproc, nil, STACK);
	ω = 1;
	t0 = nsec();
	for(;;){
		enum {MOUSE, RESIZE};
		Alt a[] = {
			{mctl->c, &mctl->Mouse, CHANRCV},
			{mctl->resizec, nil, CHANRCV},
			{nil, nil, CHANNOBLK}
		};
		wlock(&worldlock);
		switch(alt(a)){
		case MOUSE: mouse(); break;
		case RESIZE: resize(); break;
		}
		Δt = (nsec()-t0)/1e9;
		handlekeys();
		//θ = ω*Δt;
		t0 += Δt*1e9;
		wunlock(&worldlock);
		sleep(16);
	}
}
