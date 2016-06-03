
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

//##########################################
// 
// PCPC _ Assignment 08
// Maximilian Weß && Christoph Rauterberg
//
//##########################################

// Moep? Solution so simple as to simply but a dynamic for loop around it?
// DONE ?

void factorial(int x[], int n);

int main (int argc, const char * argv[])
{
    int arr[8];
    factorial(arr, 8);
    
    return 0;
}

void factorial(int x[], int n)
{
    int i,j,thread_id=42;
    
    //#pragma omp parallel for num_threads(4) \
    	default(none) private(i,j,thread_id) shared (n,x) 
    #pragma omp parallel default(none) private(i,j,thread_id) shared (n,x) 
	{
	    #pragma omp for schedule(dynamic)
	    for(i=0; i<n; i++)
	    {
		if(i<2) {
		    x[i] = 1;
		} else {
		 x[i] = x[i-1]*i;
		}
		// Use OMP-Function to get thread_ID
		thread_id= omp_get_thread_num();
		printf("Thread id # %d computed factorial(%d) = %d \n",thread_id, i, x[i]);
	    }
	}
    
    for(j=0; j<n; j++)
        printf("%d\t",x[j]);
    printf("\n");
}


