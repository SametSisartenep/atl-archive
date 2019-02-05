typedef struct Vec Vec;
typedef double Matrix[4][4];
typedef struct Quaternion Quaternion;

struct Vec {
	double	x, y, z;
};

struct Quaternion {
	double	r, i, j, k;
};
