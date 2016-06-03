//compile with
// gcc -fopenmp openmp.c

// try with different OMP_NUM_THREADS environment variables

#include <omp.h>
#include <stdio.h>

int main ()  {
int nthreads, tid;

/* do something in parallel: */
#pragma omp parallel private(tid)
  {

  /* Obtain and print thread id */
  tid = omp_get_thread_num();
  printf("Hello World from thread = %d\n", tid);

  /* Only master thread does this */
  if (tid == 0) 
    {
    nthreads = omp_get_num_threads();
    printf("Number of threads = %d\n", nthreads);
    }

  }  /* All threads join master thread and terminate */
  return 0;
}
