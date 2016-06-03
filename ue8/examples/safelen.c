#include <math.h>
#include <stdlib.h>
#define N 40000


int main ()
{

float* x[N], *y[N];

#pragma omp parallel
  {
	#pragma omp for
    for (int i = 0; i < N; i++)
	{
		x[i] = (float*) malloc(N*sizeof(float));
		y[i] = (float*) malloc(N*sizeof(float));

		#pragma omp simd safelen(18)
		for (int j = 0; j < 18; j++)
		{
			y[i][j] = x[i][j] = 0;
		}


		#pragma omp simd safelen(18)
		for (int j = 18; j < N - 18; j++)
		{
			x[i][j] = x[i][j - 18] +  (y[i][j]);
			y[i][j] = y[i][j + 18] +  (x[i][j]);
		}

		free(x[i]);
		free(y[i]);
	}
  }

  return 0;
}
