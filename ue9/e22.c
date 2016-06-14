#include <stdio.h>
#include <string.h>
#include <mpi.h>
#include <stdlib.h>

#define RINGSIZE 1
#define MSGSIZE 3

/*
char *allocStringMem(int curSize)
{
	char *tmpstring = malloc(sizeof(char)*(MSGSIZE+curSize));
	return tmpstring;
}

void strcopy(char *old, char *new, int len)
{
	int i = 0;
	for(i = 0; i<len; i++)
	{
		new[i] = old[i];
	}

}

char *addSuffix(int num, char *msg)
{
	char sufo[4] = "foo\0";
	char sufe[4] = "bar\0";
	if(num==0)
	{
		strcat(msg,sufo);
	}
	else
	{
		strcat(msg,sufe);
	}
	return msg;
}
*/
int getlen(char *s)
{
	int i = 0;
	while(s[i] != '\0')
	{
		i++;
	}
//	printf("Laenge ist %d\n", i);
	return i+1;
}
char *addString(int num, char *msg)
{

	char *sufo = "foo";
	char *sufe = "bar";
	int len = getlen(msg);
	msg = (char *)realloc(msg,sizeof(char)*(len+MSGSIZE));
	if(num!=0 && num!=1)
	{
		printf("Irgendein Fehler\n");
	}
	if(num==0)
	{
		strcat(msg,sufo);
	}
	else
	{
		strcat(msg,sufe);
	}
	return msg;
}



int main(int argc, char **argv)
{
	int ringcounter = 0;
	char *msg = NULL;
	int len = 0;
	int reclen = 0;
	int numtasks;
	int tag = 0;
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
	msg = malloc(sizeof(char)*1);
	msg[0] = '\0';
	if(rank==0)
	{
		msg = addString(rank%2,msg);
		len = getlen(msg);
		MPI_Send(msg, len, MPI_CHAR, nextrank, tag, MPI_COMM_WORLD);
	}

	//first do a receive
	while(ringcounter < RINGSIZE)
	{
		MPI_Probe(prevrank,tag,MPI_COMM_WORLD,&status);
		MPI_Get_count(&status, MPI_CHAR, &reclen);
		free(msg);
		msg = (char *) malloc(sizeof(char)*reclen);
		MPI_Recv(msg, reclen, MPI_CHAR, prevrank, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		msg = addString(rank%2,msg);
		len = getlen(msg);
		ringcounter++;
		MPI_Send(msg, len, MPI_CHAR, nextrank, tag, MPI_COMM_WORLD);
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
	if(rank==numtasks-1)
	{
		printf("%s\n", msg);
	}
	free(msg);

	MPI_Finalize();
}
