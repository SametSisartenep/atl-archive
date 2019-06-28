#include <u.h>
#include <libc.h>
#include "../geometry.h"

/* 2D */

void
addm(Matrix a, Matrix b)
{
	int i, j;

	for(i = 0; i < 3; i++)
		for(j = 0; j < 3; j++)
			a[i][j] += b[i][j];
}

void
subm(Matrix a, Matrix b)
{
	int i, j;

	for(i = 0; i < 3; i++)
		for(j = 0; j < 3; j++)
			a[i][j] -= b[i][j];
}

void
mulm(Matrix a, Matrix b)
{
	int i, j, k;
	Matrix tmp;

	for(i = 0; i < 3; i++)
		for(j = 0; j < 3; j++){
			tmp[i][j] = 0;
			for(k = 0; k < 3; k++)
				tmp[i][j] += a[i][k]*b[k][j];
		}
	for(i = 0; i < 3; i++)
		for(j = 0; j < 3; j++)
			a[i][j] = tmp[i][j];
}

void
transposem(Matrix m)
{
	int i, j;
	double tmp;

	for(i = 0; i < 3; i++)
		for(j = i; j < 3; j++){
			tmp = m[i][j];
			m[i][j] = m[j][i];
			m[j][i] = tmp;
		}
}

double
detm(Matrix m)
{
	return m[0][0]*(m[1][1]*m[2][2] - m[1][2]*m[2][1])+
	       m[0][1]*(m[1][2]*m[2][0] - m[1][0]*m[2][2])+
	       m[0][2]*(m[1][0]*m[2][1] - m[1][1]*m[2][0]);
}

void
identity(Matrix m)
{
	memset(m, 0, 3*3*sizeof(double));
	m[0][0] = m[1][1] = m[2][2] = 1;
}

Point2
xform(Point2 p, Matrix m)
{
	return (Point2){
		p.x*m[0][0] + p.y*m[0][1] + p.w*m[0][2],
		p.x*m[1][0] + p.y*m[1][1] + p.w*m[1][2],
		p.x*m[2][0] + p.y*m[2][1] + p.w*m[2][2]
	};
}

/* 3D */

void
addm3(Matrix3 a, Matrix3 b)
{
	int i, j;

	for(i = 0; i < 4; i++)
		for(j = 0; j < 4; j++)
			a[i][j] += b[i][j];
}

void
subm3(Matrix3 a, Matrix3 b)
{
	int i, j;

	for(i = 0; i < 4; i++)
		for(j = 0; j < 4; j++)
			a[i][j] -= b[i][j];
}

void
mulm3(Matrix3 a, Matrix3 b)
{
	int i, j, k;
	Matrix3 tmp;

	for(i = 0; i < 4; i++)
		for(j = 0; j < 4; j++){
			tmp[i][j] = 0;
			for(k = 0; k < 4; k++)
				tmp[i][j] += a[i][k]*b[k][j];
		}
	for(i = 0; i < 4; i++)
		for(j = 0; j < 4; j++)
			a[i][j] = tmp[i][j];
}

void
transposem3(Matrix3 m)
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
detm3(Matrix3 m)
{
	return m[0][0]*(m[1][1]*(m[2][2]*m[3][3] - m[2][3]*m[3][2])+
			m[1][2]*(m[2][3]*m[3][1] - m[2][1]*m[3][3])+
			m[1][3]*(m[2][1]*m[3][2] - m[2][2]*m[3][1]))
	      -m[0][1]*(m[1][0]*(m[2][2]*m[3][3] - m[2][3]*m[3][2])+
			m[1][2]*(m[2][3]*m[3][0] - m[2][0]*m[3][3])+
			m[1][3]*(m[2][0]*m[3][2] - m[2][2]*m[3][0]))
	      +m[0][2]*(m[1][0]*(m[2][1]*m[3][3] - m[2][3]*m[3][1])+
			m[1][1]*(m[2][3]*m[3][0] - m[2][0]*m[3][3])+
			m[1][3]*(m[2][0]*m[3][1] - m[2][1]*m[3][0]))
	      -m[0][3]*(m[1][0]*(m[2][1]*m[3][2] - m[2][2]*m[3][1])+
			m[1][1]*(m[2][2]*m[3][0] - m[2][0]*m[3][2])+
			m[1][2]*(m[2][0]*m[3][1] - m[2][1]*m[3][0]));
}

void
identity3(Matrix3 m)
{
	memset(m, 0, 4*4*sizeof(double));
	m[0][0] = m[1][1] = m[2][2] = m[3][3] = 1;
}

Point3
xform3(Point3 p, Matrix3 m)
{
	return (Point3){
		p.x*m[0][0] + p.y*m[0][1] + p.z*m[0][2] + p.w*m[0][3],
		p.x*m[1][0] + p.y*m[1][1] + p.z*m[1][2] + p.w*m[1][3],
		p.x*m[2][0] + p.y*m[2][1] + p.z*m[2][2] + p.w*m[2][3],
		p.x*m[3][0] + p.y*m[3][1] + p.z*m[3][2] + p.w*m[3][3],
	};
}
