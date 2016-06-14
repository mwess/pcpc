#include <stdio.h> // for printf
#include <stdlib.h> // for rand
#include <mpi.h>

#define MAXSTEPS 50
// create random number between min and max (inclusive)

int *
allocVec(int num)
{
	int *vec = (int *) malloc(sizeof(int)*num);
	return vec;
}

int **
allocMat(int tot, int maxstep)
{
	int i;
	int **mat = (int **) malloc(sizeof(int *)*tot);
	for(i=0;i<tot;i++)
	{
		mat[i] = (int *)malloc(sizeof(int)*maxstep);
	}
	return mat;
}


int rand_nr(int min, int max) 
{
    int nr = (rand()/(RAND_MAX + 1.0))*(max+1-min) + min;
    return nr;
}

void
copyvec2mat(int **sailormat,int *ssteps,int i)
{
	int j;
	for(j=0;j<MAXSTEPS;j++)
	{
		sailormat[i][j] = ssteps[j];
	}
}


void 
drunkensailor(int rank, int numtasks, int tag)
{
    srand(42+rank); // initialise rng
    int sailorpos = 25,i,j;
	int *sailorsteps = allocVec(MAXSTEPS);
	int *longarray = NULL;
    //printf("%d\n", sailorpos + 1);
    
    for (i = 0; i < MAXSTEPS; i++) {
        int units_to_move = rand_nr(1,10);
        int left_or_right = rand_nr(0,1);
        if (left_or_right) {
            sailorpos += units_to_move;
        }
        else {
            sailorpos -= units_to_move;
        }

        // -1 % 51 == -1 so we just always add 51...
        sailorpos = (sailorpos + 51) % 51;
        sailorsteps[i] = sailorpos + 1;
    }
    if(rank == 0)
    {
    	longarray = allocVec(numtasks*MAXSTEPS);
    }
    MPI_Gather(sailorsteps,MAXSTEPS,MPI_INT,longarray,MAXSTEPS,MPI_INT,0,MPI_COMM_WORLD);
    if(rank==0)
    {
    	printf("%d\t", i);
    	for(j=0; j<numtasks*MAXSTEPS; j++)
    	{
    		printf("%d\n", longarray[j]);
    	}
    }
	//MPI_Barrier(MPI_COMM_WORLD);
   	//return sailorsteps;
}

void
printmat(int **mat,int num)
{
	int i,j;
	for(i = 0; i < MAXSTEPS; i++)
	{
		printf("%d\t", i);
		for(j = 0; j < num; j++)
		{
			printf("%d\t", mat[j][i]);
		}
		printf("\n");
	}
}

void
freemat(int **mat, int num)
{
	int i;
	for(i=0; i<num; i++)
	{
		free(mat[i]);
	}
	free(mat);
}

int main(int argc, char **argv)
{
    srand(42); // initialise rng
 	int i;
	int mstep = MAXSTEPS;
	int **sailormat = NULL;
	int *ssteps = NULL;
	int rank, numtasks, rc, tag = 0, root = 0;
	MPI_Status status;

	rc = MPI_Init(NULL,NULL);
	if(rc != MPI_SUCCESS)
	{
		printf("Something went wrong...\n");
		MPI_Abort(MPI_COMM_WORLD, rc);
	}

	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	
	drunkensailor(rank,numtasks,tag);
	
	/*
	if(rank != 0)
	{
		MPI_Send(ssteps,MAXSTEPS,MPI_INT,0,tag,MPI_COMM_WORLD);
		free(ssteps);
	}
	else
	{
		sailormat = allocMat(numtasks,mstep);
		copyvec2mat(sailormat,ssteps,0);
		for(i=1; i<numtasks;i++)
		{
			MPI_Recv(ssteps, MAXSTEPS, MPI_INT, i, tag, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
			copyvec2mat(sailormat,ssteps,i);
		}
		printmat(sailormat,numtasks);
		freemat(sailormat,numtasks);
		free(ssteps);
	}*/

	MPI_Finalize();
    return 0;
}
