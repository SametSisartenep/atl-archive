#define Grav -9.81

enum {
	STACK	= 8192,
	PIX2M	= 4
};

typedef struct Vec Vec;
typedef struct Particle Particle;

struct Vec {
	double	x;
	double	y;
};

struct Particle {
	Vec	p;	/* position */
	Vec	v;	/* velocity */
	double	m;	/* mass */
};
