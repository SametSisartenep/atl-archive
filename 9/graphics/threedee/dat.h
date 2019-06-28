#define DEG 0.01745329251994330

enum {
	STACK = 8192,
	SEC = 1000,
	FPS = 60,
	FOV = 90
};

typedef struct Vector Vector;
typedef struct Vector3 Vector3;
typedef double Matrix[4][4];
typedef struct Triangle Triangle;
typedef struct Triangle3 Triangle3;
typedef struct Mesh Mesh;

struct Vector {
	double x, y;
};

struct Vector3 {
	double x, y, z;
};

struct Triangle {
	Point p0, p1, p2;
};

struct Triangle3 {
	Vector3 p0, p1, p2;
	Image *tx;
};

struct Mesh {
	Triangle3 *tris;
	int ntri;
};
