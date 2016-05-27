/* This program is a demonstrates the problem of sychronization 
   of multiple threads at one point. In some parallel computations
   there is the need for a "meeting point" for all threads before 
   continuing.
   
   The dot product of two vectors a and b is calculated, whereas
   each thread is responsible for a certain range of the vectors.
   After the computation, each thread calculates its relative
   part of the result.

*/

// Extended by Fatih Berber on 25.05.15
// Copyright (c) 2015 GWDG. All rights reserved.


#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

/* Define global data where everyone can see them */
#define NUMTHRDS 8
#define VECLEN 100000
int *a, *b; 
long sum=0;

/*-----------------------------------------------------------*/

/*----define here the barrier methods and struct-------------*/
typedef struct {
    int num;
    int current;
    pthread_mutex_t* mutexe;
} my_barrier_t;

void my_barrier_create(my_barrier_t* barrier, int NUM){
    barrier->num = NUM;
    barrier->current = 0;
    barrier->mutexe = (pthread_mutex_t*) malloc(sizeof(pthread_mutex_t)*barrier->num);
}

void my_barrier_wait(my_barrier_t* barrier){
    pthread_mutex_lock(&barrier->mutexe[barrier->current]);
    barrier->current++;
    if(barrier->current==barrier->num){
        for(int i = 0; i < barrier->num; i++){
            pthread_mutex_unlock(&barrier->mutexe[i]);
        }
    }
}

void my_barrier_destroy(my_barrier_t* barrier){
    free(barrier->mutexe);
}

my_barrier_t barrier;
pthread_mutex_t mutex;

/*-----------------------------------------------------------*/


void *dotprod(void *arg)
{
  /* Each thread works on a different set of data.
   * The offset is specified by the arg parameter. The size of
   * the data for each thread is indicated by VECLEN.
   */
   int i, start, end, offset, len;
   long tid = (long)arg;
   long my_sum = 0;
   double my_part;
   offset = tid;
   len = VECLEN;
   start = offset*len;
   end   = start + len;
/* Perform my section of the dot product */
   printf("thread: %ld starting. start=%d end=%d\n",tid,start,end-1);
   for (i=start; i<end ; i++){
      pthread_mutex_lock(&mutex);
      my_sum += (a[i] * b[i]); 
      sum += (a[i] * b[i]);
      pthread_mutex_unlock(&mutex);
  }
   
   printf("thread: %ld proceeding to barrier \n",tid);   

   my_barrier_wait(&barrier);
   my_part = 100*my_sum / sum;
  
   printf("thread: %ld done. My part of the Global sum is = %.1lf %% \n",tid,my_part);

   pthread_exit((void*) 0);
}


void init_pthread_variables(){

    my_barrier_create(&barrier,NUMTHRDS);
    pthread_mutex_init(&mutex,NULL);

}



int main (int argc, char *argv[])
{
long i;
void *status;
pthread_t threads[NUMTHRDS];
pthread_attr_t attr;


/* Assign storage and initialize values */
a = (int*) malloc (NUMTHRDS*VECLEN*sizeof(int));
b = (int*) malloc (NUMTHRDS*VECLEN*sizeof(int));

for (i=0; i<VECLEN*NUMTHRDS; i++){
 a[i]= b[i]=1;
}

/* init the synchronization variables*/
init_pthread_variables();

/* Create threads as joinable, each of which will execute the dot product
 * routine. Their offset into the global vectors is specified by passing
 * the "i" argument in pthread_create().
 */
pthread_attr_init(&attr);
pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
for(i=0; i<NUMTHRDS; i++) 
   pthread_create(&threads[i], &attr, dotprod, (void *)i); 

pthread_attr_destroy(&attr);

/* Wait on the threads for final result */
for(i=0; i<NUMTHRDS; i++) 
  pthread_join(threads[i], &status);

/* After joining, print out the results and cleanup */
printf ("Final Global Sum=%li\n",sum);
my_barrier_destroy(&barrier);
free (a);
free (b);
pthread_exit(NULL);
}   

