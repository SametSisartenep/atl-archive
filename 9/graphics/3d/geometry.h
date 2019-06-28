#define DEG 0.01745329251994330

typedef struct Point2 Point2;
typedef struct Point3 Point3;
typedef double Matrix[3][3];
typedef double Matrix3[4][4];
typedef struct Quaternion Quaternion;
typedef struct ReferenceFrame ReferenceFrame;
typedef struct Triangle3 Triangle3;

struct Point2 {
	double x, y, w;
};

struct Point3 {
	double x, y, z, w;
};

struct Quaternion {
	double r, i, j, k;
};

struct ReferenceFrame {
	Point3 p;
	Point3 xb, yb, zb;
};

struct Triangle3 {
	Point3 p0, p1, p2;
};

/* Point2 */
Point2 Pt2(double, double, double);
Point2 Vec2(double, double);
Point2 addpt2(Point2, Point2);
Point2 subpt2(Point2, Point2);
Point2 mulpt2(Point2, double);
Point2 divpt2(Point2, double);
double dotvec2(Point2, Point2);
double vec2len(Point2);
Point2 normvec2(Point2);
/* Point3 */
Point3 Pt3(double, double, double, double);
Point3 Vec3(double, double, double);
Point3 addpt3(Point3, Point3);
Point3 subpt3(Point3, Point3);
Point3 mulpt3(Point3, double);
Point3 divpt3(Point3, double);
double dotvec3(Point3, Point3);
Point3 crossvec3(Point3, Point3);
double vec3len(Point3);
Point3 normvec3(Point3);
/* Matrix */
void addm(Matrix, Matrix);
void subm(Matrix, Matrix);
void mulm(Matrix, Matrix);
void transposem(Matrix);
double detm(Matrix);
void identity(Matrix);
Point2 xform(Point2, Matrix);
/* Matrix3 */
void addm3(Matrix3, Matrix3);
void subm3(Matrix3, Matrix3);
void mulm3(Matrix3, Matrix3);
void transposem3(Matrix3);
double detm3(Matrix3);
void identity3(Matrix3);
Point3 xform3(Point3, Matrix3);
/* Quaternion */
Quaternion Quat(double, double, double, double);
Quaternion Quatvec(double, Point3);
Quaternion addq(Quaternion, Quaternion);
Quaternion subq(Quaternion, Quaternion);
Quaternion mulq(Quaternion, Quaternion);
Quaternion invq(Quaternion);
double qlen(Quaternion);

/* ReferenceFrame */
Point3 rframexform(Point3, ReferenceFrame);

/* Triangle3 */
Point3 centroid(Triangle3);
