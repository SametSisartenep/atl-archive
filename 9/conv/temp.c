#include <u.h>
#include <libc.h>

enum {
	C,	/* celsius */
	F,	/* fahrenheit */
	K	/* kelvin */
};

int iscale = F;
int oscale = C;

double
c2f(double T)
{
	return T * 9/5 + 32;
}

double
c2k(double T)
{
	return T + 273.15;	
}

double
f2c(double T)
{
	return (T-32) * 5/9;
}

double
f2k(double T)
{
	return c2k(f2c(T));
}

double
k2c(double T)
{
	return T - 273.15;
}

double
k2f(double T)
{
	return c2f(k2c(T));
}

double
ident(double T)
{
	return T;
}

double (*convtab[3][3])(double) = {
 [C][C] ident,
 [C][F] c2f,
 [C][K] c2k,
 [F][C] f2c,
 [F][F] ident,
 [F][K] f2k,
 [K][C] k2c,
 [K][F] k2f,
 [K][K] ident
};

int
getscale(char c)
{
	switch(c){
	case 'C': return C;
	case 'F': return F;
	case 'K': return K;
	}
	return -1;
}

void
usage(void)
{
	fprint(2, "usage: %s [-i scale] [-o scale] temp\n", argv0);
	exits("usage");
}

void
main(int argc, char *argv[])
{
	char *s;
	double iT, oT;

	ARGBEGIN{
	case 'i':
		s = EARGF(usage());
		iscale = getscale(*s);
		if(iscale < 0)
			sysfatal("uknown input scale");
		break;
	case 'o':
		s = EARGF(usage());
		oscale = getscale(*s);
		if(oscale < 0)
			sysfatal("uknown output scale");
		break;
	default: usage();
	}ARGEND;

	if(argc < 1)
		usage();
	iT = strtod(*argv, nil);
	oT = (*convtab[iscale][oscale])(iT);
	print("%g\n", oT);
	exits(0);
}
