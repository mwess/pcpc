#include <stdio.h> // for printf
#include <stdlib.h> // for rand

#define TOTAL 4
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


int *drunkensailor(int s)
{
    srand(42+s); // initialise rng
    int sailorpos = 25,i;
	int *sailorsteps = allocVec(MAXSTEPS);
    printf("%d\n", sailorpos + 1);
    
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
        //printf("%d\t%d\n", i,sailorpos + 1);
    }
	return sailorsteps;
}

void
printmat(int **mat)
{
	int i,j;
	for(i = 0; i < MAXSTEPS; i++)
	{
		printf("%d\t", i);
		for(j = 0; j < TOTAL; j++)
		{
			printf("%d\t", mat[j][i]);
		}
		printf("\n");
	}
}

void
freemat(int **mat)
{
	int i;
	for(i=0; i<TOTAL; i++)
	{
		free(mat[i]);
	}
	free(mat);
}

int main()
{
    srand(42); // initialise rng
 	int i;
	int tot = TOTAL;
	int mstep = MAXSTEPS;
	int **sailormat = allocMat(tot,mstep);
	int *ssteps = NULL;

	for(i=0;i<TOTAL;i++)
	{
		ssteps = drunkensailor(i);
		copyvec2mat(sailormat,ssteps,i);
	}
	printmat(sailormat);
	freemat(sailormat);
	free(ssteps);
    return 0;
}
