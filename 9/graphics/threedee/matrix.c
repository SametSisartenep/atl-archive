#include <u.h>
#include <libc.h>
#include <draw.h>
#include "dat.h"

void
addm(Matrix a, Matrix b)
{
	int i, j;

	for(i = 0; i < 4; i++)
		for(j = 0; j < 4; j++)
			a[i][j] += b[i][j];
}

void
subm(Matrix a, Matrix b)
{
	int i, j;

	for(i = 0; i < 4; i++)
		for(j = 0; j < 4; j++)
			a[i][j] -= b[i][j];
}

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
transm(Matrix m)
{
	int i, j;
	double tmp;

	for(i = 0; i < 4; i++)
		for(j = i; j < 4; j++){
			tmp = m[i][j];
			m[i][j] = m[j][i];
			m[j][i] = tmp;
		}
}

double
detm(Matrix m)
{
	return m[0][0]*(m[1][1]*m[2][2] - m[1][2]*m[2][1])
		+ m[0][1]*(m[1][2]*m[2][0] - m[1][0]*m[2][2])
		+ m[0][2]*(m[1][0]*m[2][1] - m[1][1]*m[2][0]);
}

Vector3
mulvecm(Vector3 v, Matrix m)
{
	Vector3 r;
	double w;

	r.x = v.x * m[0][0] + v.y * m[0][1] + v.z * m[0][2] + m[0][3];
	r.y = v.x * m[1][0] + v.y * m[1][1] + v.z * m[1][2] + m[1][3];
	r.z = v.x * m[2][0] + v.y * m[2][1] + v.z * m[2][2] + m[2][3];
	w = v.x * m[3][0] + v.y * m[3][1] + v.z * m[3][2] + m[3][3];

	if(w == 0)
		return r;
	r.x /= w;
	r.y /= w;
	r.z /= w;
	return r;
}
