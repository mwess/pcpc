#include <stdio.h>
#include <cuda.h>
#include <stdlib.h>

#define N 32

// Kernel definition
__global__ void MatAdd(int A[N][N], int B[N][N], int C[N][N])
{
	//how to get the element index, fill out the values of indexi and indexj
	int indexi = threadIdx.x + blockIdx.x*blockDim.x;
	int indexj = threadIdx.y + blockIdx.y*blockDim.y;
	if (indexi < N && indexj < N)
		C[indexi][indexj] = A[indexi][indexj] + B[indexi][indexj];
}

int main() {

	int a_h[N][N], b_h[N][N], c_h[N][N];
	int (*a_d)[N], (*b_d)[N], (*c_d)[N];

	int size = N*N*sizeof(int);

	//allocate the memory on the device
	cudaMalloc((void**)&a_d, size);
	cudaMalloc((void**)&b_d, size);
	cudaMalloc((void**)&c_d, size);

	//assign values to matrixes
	for(int i=0; i<N; i++)
		for(int j=0; j<N; j++)
		{
			a_h[i][j] = j;
			b_h[i][j] = i;
		}

	for(int i=0;i<N;i++){
   		for(int j=0;j<N;j++){
        		printf("%d ", a_h[i][j]);
    		}	
    		printf("\n\n");
	}
	for(int i=0;i<N;i++){
   		for(int j=0;j<N;j++){
        		printf("%d ", b_h[i][j]);
    		}	
    		printf("\n\n");
	}

	//copy matrixes to the device
	cudaMemcpy(a_d, a_h, size, cudaMemcpyHostToDevice);
	cudaMemcpy(b_d, b_h, size, cudaMemcpyHostToDevice);
	cudaMemcpy(c_d, c_h, size, cudaMemcpyHostToDevice);

	// launch kernel on the device with 1 block of N*N threads
	int threadsPerBlock = 16;
	int nThreads = 4;
	dim3 numBlocks(N/nThreads, N/nThreads);
	dim3 numThreads(nThreads,nThreads);
	MatAdd<<<numBlocks, numThreads>>>(a_d, b_d, c_d);

	//copy results from the device to the host
	cudaMemcpy(c_h, c_d, size, cudaMemcpyDeviceToHost);



	//print the results
	for(int i=0;i<N;i++){
   		for(int j=0;j<N;j++){
        		printf("%d ", c_h[i][j]);
    		}	
    		printf("\n");
	}

	//free the memory
	cudaFree(a_d); 
	cudaFree(b_d); 
	cudaFree(c_d);

	return 0;
}
