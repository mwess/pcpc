#include <omp.h>
#include <stdio.h>
#define N     1000
int main ()
{
int i;
float a[N], b[N], c[N], d[N];

/* Some initializations */
for (i=0; i < N; i++) {
  a[i] = i * 1.5;  b[i] = i + 22.35;
  }

#pragma omp parallel shared(a,b,c,d) private(i)
  {
  printf("I am created and I am thread %d\n",omp_get_thread_num());
  
  #pragma omp sections nowait
    {

    #pragma omp section
    {
    printf("I do sec 1 and I am thread %d\n",omp_get_thread_num());
    for (i=0; i < N; i++)
      c[i] = a[i] + b[i];
    }

    #pragma omp section
    {
    printf("I do sec 2 and I am thread %d\n",omp_get_thread_num());
    for (i=0; i < N; i++)
      d[i] = a[i] * b[i];
    }

    }  /* end of sections */

  }  /* end of parallel section */

  return 0;
}
