#include <u.h>
#include <libc.h>

#define N 4

typedef double Matrix[N][N];

void
transpose(Matrix a)
{
	int i, j;
	double tmp;

	for(i = 0; i < N; i++)
		for(j = i; j < N; j++){
			tmp = a[i][j];
			a[i][j] = a[j][i];
			a[j][i] = tmp;
		}
}

void
printm(Matrix m)
{
	int i, j;

	for(i = 0; i < N; i++){
		for(j = 0; j < N; j++)
			print("%g ", m[i][j]);
		print("\n");
	}
	print("\n");
}

void
main()
{
	Matrix m = {
		11, 12, 13, 14,
		21, 22, 23, 24,
		31, 32, 33, 34,
		41, 42, 43, 44,
	};

	printm(m);
	transpose(m);
	printm(m);
	exits(0);
}
