#include <stdio.h>
#include "cuda.h"

#define DIM 20
#define TURNS 1


__global__ void gridmean(float grid[DIM][DIM], float tmp_grid[DIM][DIM]){
	int x = blockIdx.x * blockDim.x;
	int y = blockIdx.y * blockDim.y;

	float tmp = 0;
	for(int i = x-1; i <= x+1; i++){
		for(int j = y-1; j <= y+1; j++){
			if(!(i < 0 || j < 0|| i >= DIM || j >= DIM)){
				tmp += grid[i][j];
			}	
		}
	}
	tmp_grid[x][y] = tmp/9;

}

__global__ void copy(float grid[DIM][DIM], float tmp_grid[DIM][DIM]){
	grid[blockIdx.x][blockIdx.y] = tmp_grid[blockIdx.x][blockIdx.y];
}

int main(){
	
	float grid[DIM][DIM];
	float tmp_grid[DIM][DIM];
	float (*grid_d)[DIM];
	float (*tmp_grid_d)[DIM];
	int size = DIM*DIM*sizeof(float);
	cudaMalloc((void**)&grid_d,size);
	cudaMalloc((void**)&tmp_grid_d,size);

	for(int i = 0; i < DIM; i++){
		for(int j = 0; j < DIM; j++){
			grid[i][j] = (float) i*DIM + j;
		}
	}
	
	//for(int i = 0; i < DIM; i++){
	//	for(int j = 0; j < DIM; j++){
	//		printf("%f ", grid[i][j]);
	//	}
	//	printf("\n");
	//}

	cudaMemcpy(grid_d,grid,size, cudaMemcpyHostToDevice);
	cudaMemcpy(tmp_grid_d,tmp_grid,size, cudaMemcpyHostToDevice);
	dim3 numBlocks(DIM,DIM);

	for(int k = 0; k < TURNS; k++){
		gridmean<<<numBlocks,1>>>(grid_d,tmp_grid_d);
		copy<<<numBlocks,1>>>(grid_d,tmp_grid_d);
	}

	cudaMemcpy(grid,grid_d,size, cudaMemcpyDeviceToHost);

	for(int i = 0; i < DIM; i++){
		for(int j = 0; j < DIM; j++){
			printf("%f ", grid[i][j]);
		}
		printf("\n");
	}
	cudaFree(grid_d);
	return 0;
}
