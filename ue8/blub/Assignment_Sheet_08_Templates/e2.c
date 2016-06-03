#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

//##########################################
// 
// PCPC _ Assignment 08
// Maximilian Weß && Christoph Rauterberg
//
//##########################################

// DONE

int main (int argc, char *argv[]) 
{
int   i, n;
float a[100], b[100], sum; 

/* Some initializations */
n = 100;
for (i=0; i < n; i++)
  a[i] = b[i] = i * 1.0;
sum = 0.0;
#pragma omp parallel default(shared) private(i) reduction(+:sum)
{
  #pragma omp for schedule(dynamic)
  for (i=0; i < n; i++)
    sum = sum + (a[i] * b[i]);
}
printf("   Sum = %f\n",sum);

return 0;
}
