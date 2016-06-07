#include "stdio.h"
#include "stdlib.h"
#include "omp.h"

//Execute as: ./q2 6 (command line param assigns size of two square matrices to be multiplied)
int main(int argc, char *argv[])
{
    long ** m1, ** m2, ** prod;
    long tmp;
    double start, end;
    double mystart, myend;
    int p = 0;
    int i,j,k,r1,c1,r2,c2, result=0, thread_id=42, number_of_threads=23;
	
	if ( argc != 2 && argc != 3 ) {
		printf("Correct number of arguments is required. Either \n./e4 n --> with n being rows and columns for squared matrix or \n./e4 n p --> with n being rows and columns for squared matrix and p either 0 for no output or not 0 for debug output\n");
		return 0;
	}

    r1 = c1 = r2 = c2 = atoi(argv[1]); //Input from command line argument. Restricting to square matrices.
    if (argc==3) {
      p = atoi(argv[2]);
    }
    
    
    
    if(r2==c1)
    {
        //Allocating memory
        m1 = (long **) malloc(r1 * sizeof(long *));
        for(i = 0; i < r1; i++)
        {
            m1[i] = (long *) malloc(c1 * sizeof(long));
        }
        
        m2 = (long **) malloc(r2 * sizeof(long *));
        for(i = 0; i < r2; i++)
        {
            m2[i] = (long *) malloc(c2 * sizeof(long));
        }
        
        prod = (long **) malloc(r1 * sizeof(long *));
        for(i = 0; i < r1; i++)
        {
            prod[i] = (long *) malloc(c2 * sizeof(long));
        }
        
        //Populate the matrices & print out to see it is correctly allocated memory.
	// We can parallize this as well
        printf("Populating the matrices ...\n");
	//#pragma omp parallel default(shared) private(i,j)
	{
		//#pragma omp for schedule(dynamic)
		for(i=0; i<r1; i++)
		{
		    for(j=0; j<c1; j++){
		        m1[i][j] = j+1;
		    }
		}
		//#pragma omp for schedule(dynamic)
		for(i=0; i<r2; i++)
		{
		    for(j=0; j<c2; j++){
		        m2[i][j] = j+2;
		    }
		}
	} /* end of parallel section */
	// Print Matrices
	// ###############################################
 	if ( p != 0 ) {
		printf("Matrix 1:\n");
		for( i = 0; i <r1; i ++) {
			for( j = 0; j < c1 ; j++) {
				printf("%lu\t",m1[i][j]);
			}
			printf("\n");
		}
		printf("Matrix 2:\n");
		for( i = 0; i <r2; i ++) {
			for( j = 0; j < c2 ; j++) {
				printf("%lu\t",m2[i][j]);
			}
			printf("\n");
		}
	}
	// ###############################################
	printf("Matrices populated, proceeding to multiply ...\n");
	
	for( int numbThreads = 8; numbThreads <= 256; numbThreads *= 2) {
		printf("Start computing with %d x %d matrices with %d threads!\n",r1,c1,numbThreads);
		omp_set_num_threads(numbThreads);
		start = omp_get_wtime ();
		#pragma omp parallel default(shared) private(i,j,k,tmp,mystart,myend)
		{ /* start of parallel section */
			//printf("I am thread#%d\n",omp_get_thread_num());
			mystart = omp_get_wtime ();
			#pragma omp for schedule(dynamic)
			for(i=0;i<r1;i++)
			{
				//mystart = omp_get_wtime ();
				for(j=0;j<c2;j++)
				{
				// Do not need temporary variable, as i,j,k are private and not shared.
				prod[i][j] = 0;
				    for(k=0;k<c1;k++)
				        prod[i][j] += m1[i][k]*m2[k][j];
				    // Get right Thread ID and Number of Threads using OMP functions
				thread_id = omp_get_thread_num();
				number_of_threads = omp_get_num_threads();
				if ( p != 0 ) {		        
					printf("Thread#%d of %d computed prod[%d][%d]=%lu\t\n",thread_id,number_of_threads,i,j,prod[i][j]);
				}
				}
				if ( p != 0)
					printf("\n");
			}
			myend = omp_get_wtime ();
			//printf("I am thread#%d, and my computation took %f sec.\n",omp_get_thread_num(),(myend-mystart));
		} /* end of parallel section */
		// ###############################################

		end = omp_get_wtime ();
		printf("Time Taken for Computation =%f sec\n", (end-start));
	}
        // ###############################################
        //Print the product
		if ( p != 0 ) {
		    printf("\nMultiplied Matrix:\n");
		    for(i=0; i<r1; i++)
		    {
		        for(j=0; j<c2; j++){
		            printf("%lu\t",prod[i][j]);
		        }
		        printf("\n");
		    }
        }
        //Free memory
        for(i=0;i<r1;i++) {
            free(m1[i]);
        }
        free(m1);
        for(i=0;i<r2; i++) {
            free(m2[i]);
        }
        free(m2);
        for(i=0;i<r1; i++) {
            free(prod[i]);
        }
        free(prod);
        
    }
    else
    {
        printf("Matrix multiplication cannot be done");
    }
    
    return 0;
}

