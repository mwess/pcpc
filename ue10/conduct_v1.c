#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>
#include <string.h>

#define DEFAULT_GRIDSIZE 128
#define PRINTSTEP 30
#define TIMESTEPS 1000

void init_cells(double* grid, int gridsize);
char *read_string( int argc, char **argv, const char *option, char *default_value );
int read_int( int argc, char **argv, const char *option, int default_value );
int find_option( int argc, char **argv, const char *option );

int
main(int argc, char **argv)
{
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int mpi_lattice_size = lround(sqrt(size));

    if(size < 4 || ! (mpi_lattice_size*mpi_lattice_size==size) ) 
    {
        printf("You have to use a square number of MPI processes.");
        MPI_Abort(MPI_COMM_WORLD,1);
    }

    int gridsize = read_int(argc,argv, "-n", DEFAULT_GRIDSIZE);
    if(gridsize%2 || gridsize%(mpi_lattice_size)) {
        printf("Only even gridsizes and multiples\n");
        MPI_Abort(MPI_COMM_WORLD,1);
    }
    FILE *f;

    if(rank==0) {
        char *savename = read_string( argc, argv, "-o", "sample_conduct.txt" );
        f = savename ? fopen( savename, "w" ) : NULL;
        if( f == NULL )
        {
            printf( "failed to open %s\n", savename );
            MPI_Abort(MPI_COMM_WORLD,1);
        }
    }

    int my_i, my_j;
    //int my_gridsize = GRIDSIZE/(mpi_lattice_size);
    int i,j;
    double *T, *Tn, *Tf;

    int padded_grid_size = gridsize + 2;
    Tf = (double *) malloc(gridsize*gridsize*sizeof(double));
    
    printf("Trouble!!!!\n");
    if(rank==0)
    {
        printf("Here\n");
        init_cells(Tf,gridsize);
        printf("Init works!!\n");
        MPI_Bcast(Tf,gridsize*gridsize,MPI_DOUBLE,0,MPI_COMM_WORLD);
    }
    else
    {
        MPI_Bcast(Tf,gridsize*gridsize,MPI_DOUBLE,0,MPI_COMM_WORLD);
    }
    if(rank==1)
    {
        printf("My process id is: %d\n", rank);
        for(i = 0; i < gridsize*gridsize; i++)
        {
            printf("%f\t", Tf[i]);
        }
    }
    printf("Over!!!\n");
    MPI_Finalize();
    return 0;
}

void init_cells(double* grid, int gridsize) {
  int i,j;
  
  // set everything to zero
  for(i=0;i<gridsize;i++) {
    for(j=0;j<gridsize;j++) {
      grid[i*gridsize + j]=i+j;
    }
  }
  printf("Somethin happens here!!!!\n");
  
  // but the most inner 4 cells
  /*
  for(i=gridsize/2-8;i<=gridsize/2+7;i++) {
    for(j=gridsize/2-8;j<=gridsize/2+7;j++) {
      grid[i*gridsize + j]=1;
    }
  }
  */
  
}

int read_int( int argc, char **argv, const char *option, int default_value )
{
  int iplace = find_option( argc, argv, option );
  if( iplace >= 0 && iplace < argc-1 )
    return atoi( argv[iplace+1] );
  return default_value;
}

int find_option( int argc, char **argv, const char *option )
{
  int i;
  for( i = 1; i < argc; i++ )
    if( strcmp( argv[i], option ) == 0 )
      return i;
  return -1;
}


char *read_string( int argc, char **argv, const char *option, char *default_value )
{
  int iplace = find_option( argc, argv, option );
  if( iplace >= 0 && iplace < argc-1 )
    return argv[iplace+1];
  return default_value;
}
