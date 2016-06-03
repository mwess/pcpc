#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NUMTHRDS 4
#define NUMBERS 1280000000
long sum=0;

struct interval {
  long start;
  long end;
  int tid;
};

typedef struct interval intv;


pthread_mutex_t sum_mutex;

void *add(void *arg)
{
   intv *myinterval = (intv*) arg;
   long start = myinterval->start;
   long end   = myinterval->end;

   printf("I am thread %d and my interval is %ld to %ld\n",
              myinterval->tid,start,end);
   long j;
   for(j=start;j<=end;j++) {
     pthread_mutex_lock(&sum_mutex);
     sum += j;
     pthread_mutex_unlock(&sum_mutex);
   }
   
   printf("Thread %d finished\n",myinterval->tid);

   pthread_exit((void*) 0);
}



int main (int argc, char *argv[])
{

intv *intervals;
intervals = (intv*) malloc(NUMTHRDS*sizeof(intv));

void *status;
pthread_t threads[NUMTHRDS];
pthread_mutex_init(&sum_mutex,NULL);


int i;

for(i=0; i<NUMTHRDS; i++) {
  intervals[i].start = i*(NUMBERS/NUMTHRDS);
  intervals[i].end   = (i+1)*(NUMBERS/NUMTHRDS) - 1;
  intervals[i].tid=i;
  pthread_create(&threads[i], NULL, add, (void *) &intervals[i]);
} 

/* Wait on the threads for final result */
for(i=0; i<NUMTHRDS; i++) 
  pthread_join(threads[i], &status);

/* After joining, print out the results and cleanup */
printf ("Final Global Sum=%li\n",sum);
free (intervals);
pthread_exit(NULL);
}   

