
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

void factorial(int x[], int n);

int main (int argc, const char * argv[])
{
    int arr[8] = {1};
    factorial(arr, 8);
    
    return 0;
}

void factorial(int x[], int n)
{
    int i,j,thread_id=42;
    
    #pragma omp parallel for ordered num_threads(3) default(none) private(thread_id,i,j) shared (n,x) schedule(dynamic)
    for(i=0; i<n; i++)
    {
        if(i<2)
            x[i] = 1;
        else
        #pragma omp ordered
         x[i] = x[i-1]*i;
        
        thread_id= omp_get_thread_num();
        printf("Thread id # %d computed factorial(%d) = %d \n",thread_id, i, x[i]);
    }
    
    for(j=0; j<n; j++)
        printf("%d\t",x[j]);
    printf("\n");
}


