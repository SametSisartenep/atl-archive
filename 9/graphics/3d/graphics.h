typedef enum {
	Portho,		/* orthographic */
	Ppersp		/* perspective */
} Projection;

typedef struct Vertex Vertex;
typedef struct Triangle Triangle;
typedef struct Camera Camera;

struct Vertex {
	Point3 p;	/* position */
	Point3 n;	/* surface normal */
	//Image tx;	/* (?) */
};

struct Triangle {
	Point p0, p1, p2;
};

struct Camera {
	ReferenceFrame3;		/* VCS */
	Image *viewport;
	double fov;		/* vertical FOV */
	struct {
		double n, f;	/* near and far clipping planes */
	} clip;
	Projection ptype;
	Matrix3 proj;		/* VCS to viewport xform */
};

/* Triangle */
Triangle Trian(int, int, int, int, int, int);
Triangle Trianpt(Point, Point, Point);
void triangle(Image *, Triangle, int, Image *, Point);
void filltriangle(Image *, Triangle, Image *, Point);
/* Camera */
void perspective(Matrix3, double, double, double, double);
void orthographic(Matrix3, double, double, double, double, double, double);
void configcamera(Camera *, Image *, double, double, double, Projection);
void placecamera(Camera *, Point3, Point3, Point3);
void aimcamera(Camera *, Point3);
void reloadcamera(Camera *);
