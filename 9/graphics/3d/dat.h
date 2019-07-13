enum {
	STACK = 8192,
	SEC = 1000,
	FPS = 60
};

enum {
	K↑,
	K↓,
	K←,
	K→,
	Krise,
	Kfall,
	KR↑,
	KR↓,
	KR←,
	KR→,
	KR↺,
	KR↻,
	Kcam0,
	Kcam1,
	Kcam2,
	Kcam3,
	Kscrshot,
	Ke
};

enum {
	Scamno,
	Scampos,
	Sfov,
	Se
};
char stats[Se][256];

typedef struct Mesh Mesh;
typedef struct TTriangle3 TTriangle3;

struct Mesh {
	Triangle3 *tris;
	int ntri;
};

struct TTriangle3 {
	Triangle3;
	Image *tx;
};
