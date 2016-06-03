// gcc -fopenmp   examples/for.c
#include <omp.h>
#include <stdio.h>
// #define N     1000
#define N     10000

int main ()  
{
int i;
float a[N], b[N], c[N];

/* Some initializations */
for (i=0; i < N; i++)
  a[i] = b[i] = i * 1.0;

#pragma omp parallel shared(a,b,c) private(i)
  {

  #pragma omp for schedule(dynamic)
  for (i=0; i < N; i++)
    c[i] = a[i] + b[i];


  printf("hello\n");

  
  // /* only the master does printf */
  // #pragma omp master
  // {
  //     for(i=0;i<N;i++) {
  //         printf("c[%d] = %f\n",i,c[i]);
  //     }
  // }
  
  // #pragma omp for schedule(dynamic)
  // for (i=0; i < N; i++)
  //   c[i] = a[i] + b[i];
  //   printf("Test\n");

  }  /* end of parallel section */

  return 0;

}
