#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <math.h>

#define DEFAULT_GRIDSIZE 128
#define PRINTSTEP 30
#define TIMESTEPS 1000

void put_neighbor(double * T,int i,int j,int my_mpi_i,int my_mpi_j, int mpi_lattice_size,int my_gridsize);
void get_neighbor(double * T,int i,int j,int my_mpi_i,int my_mpi_j, int mpi_lattice_size,int my_gridsize);

void init_cells(double* grid, int gridsize);
void print(double* grid, int padded_grid_size, int time);
void save(FILE *f, double* grid, int padded_grid_size, int time);
int find_option( int argc, char **argv, const char *option );
int read_int( int argc, char **argv, const char *option, int default_value );
char *read_string( int argc, char **argv, const char *option, char *default_value );



int main(int argc, char** argv) {

  int rank, size;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  
  int mpi_lattice_size = lround(sqrt(size));
  
  if(size < 4 || ! (mpi_lattice_size*mpi_lattice_size==size) ) {
    printf("You have to use a square number of MPI processes.");
    MPI_Abort(MPI_COMM_WORLD,1);
  }
  
  if( find_option( argc, argv, "-h" ) >= 0 )
  {
      printf( "Options:\n" );
      printf( "-h to see this help\n" );
      printf( "-n <int> to set the grid size\n" );
      printf( "-o <filename> to specify the output file name\n" );
      MPI_Abort(MPI_COMM_WORLD,1);
  }
  
  int GRIDSIZE = read_int( argc, argv, "-n", DEFAULT_GRIDSIZE );
  // Check gridsize for some basic assumptions
  if(GRIDSIZE%2 || GRIDSIZE%(mpi_lattice_size)) {
    printf("Only even Gridsize allowed and\nGridsize has to be a multiple of the number of MPI procs!\n");
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
  

  int my_mpi_i,my_mpi_j;
  int my_gridsize= GRIDSIZE/(mpi_lattice_size);
  int t,i,j;
  double *T, *Tn,*Tf;
  
  my_mpi_j=rank%mpi_lattice_size;
  my_mpi_i=rank/mpi_lattice_size;
  
  // Allocate Grid with a border on every side
  int padded_grid_size = my_gridsize+2;
  if(rank == 0) {
    Tf=(double *) malloc(GRIDSIZE*GRIDSIZE*sizeof(double));
  }
  T=(double *) malloc((padded_grid_size)*(padded_grid_size)*sizeof(double));
  Tn=(double *) malloc((padded_grid_size)*(padded_grid_size)*sizeof(double));
  

  
  // remember -- our grid has a border around it!
  if(rank==0)
    init_cells(Tf,GRIDSIZE);
  else {
    int i,j;
    for (i=0;i<padded_grid_size;i++)
      for(j=0;j<padded_grid_size;j++)
        T[i*padded_grid_size+j]=0;
  }
  
  
  if(rank==0) {
    for(i=0;i<mpi_lattice_size;i++) {
      for(j=0;j<mpi_lattice_size;j++) {
        if(i==0 && j==0) {
          int k,l,m=1,n=1;
          for(k=0;k<padded_grid_size;k++)
            for(l=0;l<padded_grid_size;l++)
              T[k*padded_grid_size+l]=0;
          
          for(k=0;k<my_gridsize;k++) {
          
            for(l=0;l<my_gridsize;l++) {
            
              T[m*padded_grid_size+n]=Tf[k*(GRIDSIZE) + l];
              n++;
            }
            m++;
            n=1;
          }
          continue;
        }
        
        //re-use Tn for temp arrays
        int k,l,m=1,n=1;
        for(k=0;k<padded_grid_size;k++)
          for(l=0;l<padded_grid_size;l++)
            Tn[k*padded_grid_size+l]=0;
        
        for(k=0;k<my_gridsize;k++) {
          for(l=0;l<my_gridsize;l++) {
            Tn[m*padded_grid_size+n]=Tf[(k + my_gridsize*i)*(GRIDSIZE) + l+my_gridsize*j];
            n++;
          }
          m++;
          n=1;
        }
        int dest;
        dest=i*mpi_lattice_size+j;
        // printf("\nDest: %d from %d where %d\n  ",dest,size,mpi_lattice_size);
        MPI_Send(Tn,padded_grid_size*padded_grid_size,MPI_DOUBLE,dest,42,MPI_COMM_WORLD);
      }
    }
  }
  else {
    // printf("\nRank: %d %d %d\n  ",rank,my_mpi_i,my_mpi_j);
    MPI_Recv(T,padded_grid_size*padded_grid_size,MPI_DOUBLE,0,42,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
  }
  
  // printf("\nRank: %d %d %d\n  ",rank,my_mpi_i,my_mpi_j);
  // print(T,padded_grid_size,rank);
  // MPI_Barrier(MPI_COMM_WORLD);
  // exit(1);
  
  

  
  for(t=1;t<=TIMESTEPS;t++) { // Loop for the time steps
  
    // get the neighbors:
    put_neighbor(T,0,1,my_mpi_i,my_mpi_j,mpi_lattice_size,my_gridsize);
    get_neighbor(T,0,-1,my_mpi_i,my_mpi_j,mpi_lattice_size,my_gridsize);
    
    put_neighbor(T,1,0,my_mpi_i,my_mpi_j,mpi_lattice_size,my_gridsize);
    get_neighbor(T,-1,0,my_mpi_i,my_mpi_j,mpi_lattice_size,my_gridsize);
    
    put_neighbor(T,0,-1,my_mpi_i,my_mpi_j,mpi_lattice_size,my_gridsize);
    get_neighbor(T,0,1,my_mpi_i,my_mpi_j,mpi_lattice_size,my_gridsize);
    
    put_neighbor(T,-1,0,my_mpi_i,my_mpi_j,mpi_lattice_size,my_gridsize);
    get_neighbor(T,1,0,my_mpi_i,my_mpi_j,mpi_lattice_size,my_gridsize);

    
    MPI_Barrier(MPI_COMM_WORLD);    
    /*
    printf("\nI am: %d\n",rank);
    print(T,padded_grid_size,t);
    printf("\n");
    */



    for(i=1;i<my_gridsize+1;i++) {
      for(j=1;j<my_gridsize+1;j++) {
        
        Tn[i*padded_grid_size + j] = T[(i-1)*padded_grid_size + (j)] + T[(i+1)*padded_grid_size + (j)]
                                   + T[(i)*padded_grid_size + (j-1)] + T[(i)*padded_grid_size + (j+1)];
        Tn[i*padded_grid_size + j] /= 4;
      }
    }

    for(i=1;i<my_gridsize+1;i++) {
      for(j=1;j<my_gridsize+1;j++) {
        T[i*padded_grid_size + j] = Tn[i*padded_grid_size + j];
      }
    }
    
    /*
    MPI_Barrier(MPI_COMM_WORLD);    
    printf("\nI am: %d\n",rank);
    print(T,padded_grid_size,t);
    printf("\n");
    */
    
    if(!(t % PRINTSTEP)) {
    
      if(rank==0) {
        for(i=0;i<mpi_lattice_size;i++) {
          for(j=0;j<mpi_lattice_size;j++) {
            if(i==0 && j==0) {
              int k,l,m=1,n=1;
              for(k=0;k<my_gridsize;k++) {
                for(l=0;l<my_gridsize;l++) {
                  Tf[k*(GRIDSIZE) + l]=T[m*padded_grid_size+n];
                  n++;
                }
                m++;
                n=1;
              }
            }
            else {
              int source=i*mpi_lattice_size+j;
              MPI_Recv(Tn,padded_grid_size*padded_grid_size,MPI_DOUBLE,source,42,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
              //re-use Tn for temp arrays
              int k,l,m=1,n=1;
              for(k=0;k<my_gridsize;k++) {
                for(l=0;l<my_gridsize;l++) {
                  Tf[(k + my_gridsize*i)*(GRIDSIZE) + l+my_gridsize*j]=Tn[m*padded_grid_size+n];
                  n++;
                }
                m++;
                n=1;
              }
            }
          }
        }
      }
      else {
        MPI_Send(T,padded_grid_size*padded_grid_size,MPI_DOUBLE,0,42,MPI_COMM_WORLD);
      }

      if(rank==0) {
         // print(Tf,GRIDSIZE,t);
        save(f,Tf,GRIDSIZE,t);
        printf("Time: %d\n",t);
      }
    }
    
  }
  
  if(rank==0)
    fclose(f);
  
  
    
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
  
  // but the most inner 4 cells
  /*
  for(i=gridsize/2-8;i<=gridsize/2+7;i++) {
    for(j=gridsize/2-8;j<=gridsize/2+7;j++) {
      grid[i*gridsize + j]=1;
    }
  }
  */
  
}


void put_neighbor(double * T,int i,int j,int my_mpi_i,int my_mpi_j, int mpi_lattice_size,int my_gridsize) {
  if(my_mpi_i < 1 && i<0) {
    return;
  }
  if(my_mpi_i+1 >= mpi_lattice_size && i>0) {
    return;
  }
  if(my_mpi_j < 1 && j<0) {
    return;
  }
  if(my_mpi_j+1 >= mpi_lattice_size && j>0) {
    return;
  }
  
  double * sendv=(double *) malloc(my_gridsize*sizeof(double));
  int dest=(my_mpi_i+i)*mpi_lattice_size + my_mpi_j+j;
  // printf("Send: (%d,%d) to (%d,%d,%d): ",my_mpi_i,my_mpi_j,dest,i,j);
  int k,l;
  if(i) {
    if(i<0) k=1;
    else k=my_gridsize;
    
    for(l=0;l<my_gridsize;l++) {
      sendv[l]=T[k*(my_gridsize+2)+(l+1)];
      // printf("%lf ",sendv[l]);
      }
  }
  else if(j) {
    if(j<0) l=1;
    else l=my_gridsize;
    
    for(k=0;k<my_gridsize;k++){
      sendv[k]=T[(k+1)*(my_gridsize+2)+l];
      // printf("%lf ",sendv[k]);
      }
  }
  // printf("\n");
  MPI_Send(sendv,my_gridsize,MPI_DOUBLE,dest,42,MPI_COMM_WORLD);
  free(sendv);
}

void get_neighbor(double * T,int i,int j,int my_mpi_i,int my_mpi_j, int mpi_lattice_size,int my_gridsize) {
  if(my_mpi_i < 1 && i<0) {
    return;
  }
  if(my_mpi_i+1 >= mpi_lattice_size && i>0) {
    return;
  }
  if(my_mpi_j < 1 && j<0) {
    return;
  }
  if(my_mpi_j+1 >= mpi_lattice_size && j>0) {
    return;
  }
  
  int src=(my_mpi_i+i)*mpi_lattice_size + my_mpi_j+j;
  // printf("Recv: (%d,%d) from (%d,%d,%d): ",my_mpi_i,my_mpi_j,src,i,j);
  double * recv=(double *) malloc(my_gridsize*sizeof(double));
  
  
  MPI_Recv(recv,my_gridsize,MPI_DOUBLE,src,42,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
  
  int k,l;
  if(i) {
    if(i<0) k=0;
    else k=my_gridsize+1;
    
    for(l=0;l<my_gridsize;l++) {
      T[k*(my_gridsize+2)+(l+1)]=recv[l];
      // printf("%lf ",recv[l]);
    }
  }
  else if(j) {
    if(j<0) l=0;
    else l=my_gridsize+1;
    
    for(k=0;k<my_gridsize;k++) {
      T[(k+1)*(my_gridsize+2)+l]=recv[k];
      // printf("%lf ",recv[k]);
      }
  }
  // printf("\n");

  free(recv);
}


void print(double* grid, int padded_grid_size, int time) {
  printf("\n\n\n");
  int i,j;
  
  // we don't want to print the border!
  for(i=0;i<padded_grid_size;i++) {
    for(j=0;j<padded_grid_size;j++) {
      printf("%.2f ",grid[i*padded_grid_size + j]);
    }
    printf("\n");
  }
}

void save( FILE *f, double* grid, int padded_grid_size,int time)
{
    int i,j;
    
    static int first = 1;
    if( first )
    {
    fprintf( f, "# %d %d\n", TIMESTEPS, padded_grid_size );
        first = 0;
    }

    for(i = 0; i < padded_grid_size; i++ ) {
      for(j=0; j < padded_grid_size; j++) {
        fprintf( f, "%.g ", grid[i* padded_grid_size + j] );
      }
      fprintf(f,"\n");
    }

}


//
//  command line option processing
//
int find_option( int argc, char **argv, const char *option )
{
  int i;
  for( i = 1; i < argc; i++ )
    if( strcmp( argv[i], option ) == 0 )
      return i;
  return -1;
}

int read_int( int argc, char **argv, const char *option, int default_value )
{
  int iplace = find_option( argc, argv, option );
  if( iplace >= 0 && iplace < argc-1 )
    return atoi( argv[iplace+1] );
  return default_value;
}

char *read_string( int argc, char **argv, const char *option, char *default_value )
{
  int iplace = find_option( argc, argv, option );
  if( iplace >= 0 && iplace < argc-1 )
    return argv[iplace+1];
  return default_value;
}
