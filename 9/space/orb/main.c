#include <u.h>
#include <libc.h>
#include <draw.h>
#include <event.h>

#define	SEC	1000

typedef struct Vector Vector;
struct Vector
{
	int x, y;
};

Image *bg;
Point O;
Vector v1;

Point
vec2pt(Vector v)
{
	return Pt(O.x+v.x, O.y-v.y);
}

void
redraw(void)
{
	O = Pt(screen->r.min.x, screen->r.max.y);

	draw(screen, screen->r, bg, nil, ZP);
	line(screen, O, vec2pt(v1), 0, Endarrow, 1, display->white, ZP);
}

void
eresized(int new)
{
	if(new && getwindow(display, Refnone) < 0)
		fprint(2, "can't reattach to window\n");
	redraw();
}

void
main()
{
	if(initdraw(0, 0, "orbital") < 0)
		sysfatal("initdraw: %r");

	einit(Emouse);

	bg = allocimagemix(display, DWhite, DBlack);
	v1 = Pt(100, 100);

	for(;;){
		redraw();
		flushimage(display, 1);

		if(ecanmouse()) emouse();
		
		sleep(SEC/30);
	}
}
