/*
 * Implementation of the techniques described in †.
 *
 * † Tom Duff, James Burgess, Per Christensen, Christophe Hery, Andrew
 * Kensler, Max Liani, and Ryusuke Villemin, Building an Orthonormal
 * Basis, Revisited, Journal of Computer Graphics Techniques (JCGT), vol. 6,
 * no. 1, 1-8, 2017. Available online at http://jcgt.org/published/0006/01/01/
*/

#include <u.h>
#include <libc.h>

typedef struct Vec Vec;
struct Vec {
	double x, y, z;
};

vlong t0;

Vec
Vect(double x, double y, double z)
{
	return (Vec){x, y, z};
}

void
frisvadONB(Vec b[3])
{
	double p, q;

	if(b[0].z < -0.9999999){	/* handle singularity */
		b[1] = Vect(0, -1, 0);
		b[2] = Vect(-1, 0, 0);
		return;
	}
	p = 1/(1 + b[0].z);
	q = -b[0].x*b[0].y*p;
	b[1] = Vect(1 - b[0].x*b[0].x*p, q, -b[0].x);
	b[2] = Vect(q, 1 - b[0].y*b[0].y*p, -b[0].y);
}

void
revisedONB(Vec b[3])
{
	double p, q;

	if(b[0].z < 0){
		p = 1/(1 - b[0].z);
		q = b[0].x*b[0].y*p;
		b[1] = Vect(1 - b[0].x*b[0].x*p, -q, b[0].x);
		b[2] = Vect(q, b[0].y*b[0].y*p - 1, -b[0].y);
		return;
	}
	p = 1/(1 + b[0].z);
	q = -b[0].x*b[0].y*p;
	b[1] = Vect(1 - b[0].x*b[0].x*p, q, -b[0].x);
	b[2] = Vect(q, 1 - b[0].y*b[0].y*p, -b[0].y);
}

void
revisedaONB(Vec b[3])
{
	double p, q, σ;

	σ = b[0].z >= 0 ? 1 : -1;
	p = -1/(σ + b[0].z);
	q = b[0].x*b[0].y*p;
	b[1] = Vect(1 + σ*b[0].x*b[0].x*p, σ*q, -σ*b[0].x);
	b[2] = Vect(q, σ + b[0].y*b[0].y*p, -b[0].y);
}

void
usage(void)
{
	fprint(2, "usage: %s x y z\n", argv0);
	exits("usage");
}

void
main(int argc, char *argv[])
{
	Vec b[3];
	int i;

	ARGBEGIN{
	default: usage();
	}ARGEND;
	if(argc != 3)
		usage();
	b[0].x = strtod(argv[0], nil);
	b[0].y = strtod(argv[1], nil);
	b[0].z = strtod(argv[2], nil);
	//b[0] = Vect(0.00038527316, 0.00038460016, -0.99999988079);
	t0 = nsec();
	frisvadONB(b);
	print("Frisvad's (took %lldns)\n", nsec()-t0);
	for(i = 0; i < nelem(b); i++)
		print("\tB%d [%g %g %g]\n", i+1, b[i].x, b[i].y, b[i].z);
	t0 = nsec();
	revisedONB(b);
	print("Revised (took %lldns)\n", nsec()-t0);
	for(i = 0; i < nelem(b); i++)
		print("\tB%d [%g %g %g]\n", i+1, b[i].x, b[i].y, b[i].z);
	t0 = nsec();
	revisedaONB(b);
	print("Revised Adv. (took %lldns)\n", nsec()-t0);
	for(i = 0; i < nelem(b); i++)
		print("\tB%d [%g %g %g]\n", i+1, b[i].x, b[i].y, b[i].z);
	exits(nil);
}
