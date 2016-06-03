#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#define N     50

//##########################################
// 
// PCPC _ Assignment 08
// Maximilian Weß && Christoph Rauterberg
//
//##########################################

// Not quite clear what the expected solution is here....

int main (int argc, char *argv[]) 
{
int i, nthreads, tid, section;
float a[N], b[N], c[N];
void print_results(float array[N], int tid, int section);

/* Some initializations */
for (i=0; i<N; i++)
  a[i] = b[i] = i * 1.0;

#pragma omp parallel shared(c) private(i,tid,section)
  {
  // Use oMP-Functions to get number of this thread
  tid = omp_get_thread_num ();
  // Only Master (tid == 0) may enter here.
  // This way appears to be fancier
  #pragma omp master
    {
    // Use oMP-Functions to get number of threads
    nthreads = omp_get_num_threads (); 
    printf("---\nHello, this is the Master Thread speaking! \nNumber of threads = %d\n---\n", nthreads);
    }

  /*** Use barriers for clean output ***/
  #pragma omp barrier
  printf("Thread %d starting...\n",tid);
  #pragma omp barrier

      
      section = 1;
      #pragma omp for schedule(dynamic)
      for (i=0; i<N; i++)
        c[i] = a[i] * b[i];
      print_results(c, tid, section);
      

      
      section = 2;
      #pragma omp for schedule(dynamic)
      for (i=0; i<N; i++)
        c[i] = a[i] + b[i];
      print_results(c, tid, section);
      

     

  /*** Use barrier for clean output ***/
  #pragma omp barrier
  printf("Thread %d exiting...\n",tid);

  }  /* end of parallel section */

return 0;
}



void print_results(float array[N], int tid, int section) 
{
  int i,j;

  j = 1;
  /*** use critical for clean output ***/
  #pragma omp critical
  {
  printf("\nThread %d did section %d. The results are:\n", tid, section);
  for (i=0; i<N; i++) {
    printf("%e  ",array[i]);
    j++;
    if (j == 6) {
      printf("\n");
      j = 1;
      }
    }
    printf("\n");
  } /*** end of critical ***/

  #pragma omp barrier
  printf("Thread %d done and synchronized.\n", tid); 

}
