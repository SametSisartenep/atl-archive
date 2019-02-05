#define DEG 0.01745329251994330

typedef struct Vec Vec;
typedef struct Triangle Triangle;

struct Vec {
	double x, y;
};

struct Triangle {
	Point p0, p1, p2;	
};
