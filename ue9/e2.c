#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>

#define RINGSIZE 10

int main(int argc, char **argv)
{
	int ringcounter = 0;
	int msgnumber = 0;
	int numtasks;
	int tag = 43;
	int rc, rank;
	MPI_Status status;

	rc = MPI_Init(NULL,NULL);
	if (rc != MPI_SUCCESS)
	{
		printf("Something went wrong..\n");
		MPI_Abort(MPI_COMM_WORLD, rc);
	}

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);

	//now do a first send
	if(rank==0)
	{
		printf("Send first message!\n");
		MPI_Send(&msgnumber,1,MPI_INT,(msgnumber+1)%numtasks, tag, MPI_COMM_WORLD);
		printf("First message sent!\n");
	}

	//first do a receive
	while(ringcounter < RINGSIZE)
	{
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
	MPI_Finalize();

}
