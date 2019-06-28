#include <u.h>
#include <libc.h>

typedef double Matrix[4][4];

void
mulm(Matrix a, Matrix b)
{
	int i, j, k;
	Matrix r;

	for(i = 0; i < 4; i++)
		for(j = 0; j < 4; j++){
			r[i][j] = 0;
			for(k = 0; k < 4; k++)
				r[i][j] += a[i][k]*b[k][j];
		}
	for(i = 0; i < 4; i++)
		for(j = 0; j < 4; j++)
			a[i][j] = r[i][j];
}

void
printm(Matrix m)
{
	int i, j;

	for(i = 0; i < 4; i++){
		for(j = 0; j < 4; j++)
			print("%g\t", m[i][j]);
		print("\n");
	}
}

void
main()
{
	Matrix m1 = {
		1, 2, -1, 0,
		2, 0, 1, 0,
		0, 0, 0, 0,
		0, 0, 0, 0
	};
	Matrix m2 = {
		3, 1, 0, 0,
		0, -1, 0, 0,
		-2, 3, 0, 0,
		0, 0, 0, 0
	};
	mulm(m1, m2);
	printm(m1);
	exits(0);
}
