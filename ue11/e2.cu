#include <stdio.h>
#include <cuda.h>
#include <stdlib.h>

#define N 64

// Kernel definition
__global__ void MatMul(int A[N][N], int B[N][N], int C[N][N])
{
	//finish matrix multiplication, each thread calculate one element of C
	int mi = threadIdx.x + blockIdx.x*blockDim.x;
	int mj = threadIdx.y + blockIdx.y*blockDim.y;
	C[mi][mj] = 0;
	for(int i = 0; i < N; i++){
			C[mi][mj] += A[mi][i] * B[i][mj];
	}
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
			b_h[i][j] = j;
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

	// launch kernel on the device by combining two-dimensional blocks with two-dimensional threads
	//define numBlocks and threadsPerBlock
	int nThreads = 4;
	dim3 numBlocks(N/nThreads,N/nThreads);
	dim3 numThreads(nThreads,nThreads);
	MatMul<<<numBlocks, numThreads>>>(a_d, b_d, c_d);

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
