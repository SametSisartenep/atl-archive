#include <u.h>
#include <libc.h>
#include <ctype.h>
#include <draw.h>
#include <event.h>

#define	SEC		1000			/* ms */
#define	RDINTERVAL	120			/* seconds */
#define	Battery		"/mnt/acpi/battery"

enum
{
	Labspace = 2				/* room around the label */
};

Image	*bg, *juice;
int	nbatt, lvl;
char	lvlstr[5];


void
usage(void)
{
	fprint(2, "usage: battery\n");
	exits("usage");
}

void
juicereflow(double ratio)
{
	int dy = screen->r.max.y-screen->r.min.y;
	Point sp = Pt(screen->r.min.x, screen->r.max.y-(dy*ratio));

	draw(screen, Rpt(sp, screen->r.max), juice, nil, ZP);
}

void
label(Point p, char *text)
{
	string(screen, p, display->black, ZP, font, text);
}

void
redraw(void)
{
	Point p = Pt(((screen->r.min.x+screen->r.max.x)/2)-((font->width*strlen(lvlstr))/2),
		screen->r.max.y-(font->height+Labspace));

	draw(screen, screen->r, bg, nil, ZP);
	juicereflow(lvl/100.0);
	label(p, lvlstr);
}

void
eresized(int new)
{
	if(new && getwindow(display, Refnone) < 0)
		fprint(2, "can't reattach to window\n");
	redraw();
}

void
main(int argc, char *[])
{
	int fd;
	char buf[256], *s;
	long t0, t;
	/* initialize reading timers */
	t0 = t = 0;

	if(argc > 1)
		usage();

	fd = open(Battery, OREAD);
	if(fd < 0)
		sysfatal("open: %r");

	if(initdraw(0, 0, "battery") < 0)
		sysfatal("initdraw: %r");

	einit(Emouse);

	bg = allocimagemix(display, 0x88FF88FF, DWhite);
	juice = allocimagemix(display, DDarkgreen, DDarkgreen);

	for(;;){
		if((time(&t) - t0) >= RDINTERVAL){
			t0 = t;
			memset(buf, 0, sizeof buf);
			pread(fd, buf, sizeof buf, 0);
			/* remove the trailing nl to avoid listing a nil battery */
			buf[strlen(buf)-1] = '\0';
			s = buf;
			nbatt = 1;
			lvl = atoi(s);
			while(*s++ != '\0')
				if(*s == '\n'){
					nbatt++;
					lvl += atoi(++s);
				}
			lvl = lvl/nbatt;
			snprint(lvlstr, sizeof lvlstr, "%d%%", lvl);
		}
		redraw();
		flushimage(display, 1);

		/* get resize events */
		if(ecanmouse()) emouse();

		sleep(SEC/30);		/* 30 fps */
	}
}
