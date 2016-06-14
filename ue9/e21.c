#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>

#define RINGSIZE 10

int main(int argc, char **argv)
{
	int ringcounter = 0;
	int msgnumber = 1;
	int numtasks;
	int tag = 43;
	int rc, rank, nextrank, prevrank;
	MPI_Status status;

	rc = MPI_Init(NULL,NULL);
	if (rc != MPI_SUCCESS)
	{
		printf("Something went wrong..\n");
		MPI_Abort(MPI_COMM_WORLD, rc);
	}

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
	//declare some stuff
	nextrank = (rank+1)%numtasks;
	if(rank-1 < 0)
	{
		prevrank = (numtasks -1);
	}
	else
	{
		prevrank = (rank-1)%numtasks;
	}

	//now do a first send
	if(rank==0)
	{
		MPI_Send(&msgnumber, 1, MPI_INT, nextrank, tag, MPI_COMM_WORLD);
	}

	//first do a receive
	while(ringcounter < RINGSIZE)
	{
		MPI_Recv(&msgnumber, 1, MPI_INT, prevrank, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		msgnumber++;
		ringcounter++;
		printf("Rank %d got msg %d with rc %d\n", rank,msgnumber,ringcounter);
		MPI_Send(&msgnumber, 1, MPI_INT, nextrank, tag, MPI_COMM_WORLD);
	}
	/*while(ringcounter < RINGSIZE)
	{
		if(rank!=0)
		{
			printf("Waiting rank %d wich rc %d\n", rank,ringcounter);
			MPI_Recv(&msgnumber, 1, MPI_INT, prevrank, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			msgnumber++;
			if(rank==numtasks-1 )
			{
				ringcounter++;
			}
			printf("Rank %d got msg %d with ringcounter %d\n", rank,msgnumber, ringcounter);
			MPI_Send(&msgnumber, 1, MPI_INT, nextrank, tag, MPI_COMM_WORLD);
			printf("Ringcounter: %d\n", ringcounter);
		}
		else
		{	
			printf("Send first message!\n");
			MPI_Send(&msgnumber,1,MPI_INT,nextrank, tag, MPI_COMM_WORLD);
			printf("First message sent!\n");
			MPI_Recv(&msgnumber, 1, MPI_INT, prevrank, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
	}
	/*
		printf("Rank %d waiting\n", rank);
		MPI_Recv(&msgnumber, 1, MPI_INT, msgnumber%numtasks, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		printf("Message: %d\nRank: %d\n", msgnumber, (msgnumber+1)%numtasks);
		if(msgnumber%numtasks==0)
		{
			ringcounter++;
		}
		if(rank==(msgnumber+1)%numtasks)
		{
			printf("Rank %d is about to send\n", rank);
			msgnumber++;
			MPI_Send(&msgnumber,1,MPI_INT,(msgnumber+1)%numtasks, tag, MPI_COMM_WORLD);
			printf("Und verschickt auch\n");

		}
	}
	
	if(rank==0)
	{
		printf("Kommt hier was an?\n");
		MPI_Recv(&msgnumber, 1, MPI_INT, prevrank, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}
	*/
	MPI_Finalize();
}
