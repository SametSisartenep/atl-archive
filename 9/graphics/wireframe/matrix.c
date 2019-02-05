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
