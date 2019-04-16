#define DEG 0.01745329251994330
#define RAD (1/DEG)

enum {
	STACKSZ = 8192,
	SEC = 1000,
	THRUST = 50,
	FPS = 60
};

typedef struct Vector Vector;
typedef struct Triangle Triangle;
typedef struct Asteroid Asteroid;
typedef struct Bullet Bullet;
typedef struct Spacecraft Spacecraft;

struct Vector {
	double x, y;
};

struct Triangle {
	Point p0, p1, p2;
};

struct Asteroid {
	Vector p, v;
	double yaw;
	int stillin;
};

struct Bullet {
	Vector p, v;
};

struct Spacecraft {
	Vector p, v;
	double yaw;
	int shields;
};
