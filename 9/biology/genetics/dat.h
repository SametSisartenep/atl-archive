#define MOL 6.02e23

enum {
	A,	/* adenine */
	C,	/* cytosine */
	G,	/* guanine */

	T,	/* thymine */
	U	/* uracil */
};

typedef struct Atom Atom;
typedef struct Element Element;
typedef struct Pyrimidine Pyrimidine;
typedef struct Purine Purine;
typedef struct Nucleobase Nucleobase;
typedef struct Nucleotide Nucleotide;
typedef struct Gene Gene;

Gene genome[] = {};	/* ? */

struct Atom {
	int	z;	/* atomic number */
	int	nelec;	/* number of electrons */
	int	nneut;	/* number of neutrons */
	double	mass;	/* relative atomic mass */
};

struct Element {
	Atom	a;	/* atomic properties */
	char	*sym;	/* symbol */
	char	*name;
};

Atom H = { 1,  1,  1,  1.008};
Atom C = { 6,  6,  6, 12.011};
Atom N = { 7,  7,  7, 14.007};
Atom O = { 8,  8,  8, 15.999};
Atom P = {15, 15, 15, 30.974};
Atom S = {16, 16, 16, 32.060};

Element periodictab[] = {
	H, "H", "hydrogen",
	C, "C", "carbon",
	N, "N", "nitrogen",
	O, "O", "oxygen",
	P, "P", "phosphorus",
	S, "S", "sulfur"
};

struct Pyrimidine {
	
};

struct Purine {
	
};

struct Nucleobase {
	
};

struct Nucleotide {
	
};
