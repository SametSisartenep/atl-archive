typedef struct Vec Vec;
typedef struct Cube Cube;
typedef struct Sphere Sphere;

struct Vec {
	double	x, y, z;
};

struct Cube {
	Vec	min, max;
};

struct Sphere {
	Vec	p;
	double	r;
};
