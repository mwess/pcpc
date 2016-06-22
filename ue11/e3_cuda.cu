#include <stdio.h>
#include <cuda.h>
#include <stdlib.h>

#define N 64
#define R 3

// Kernel definition
__global__ void MatMul(int A[N], int B[N])
{
	//finish matrix multiplication, each thread calculate one element of C
	int mi = threadIdx.x + blockIdx.x*blockDim.x;
	for(int j = -R; j < R+1; j++){
		if(!(mi+j < 0 || mi+j >= N)){
			B[mi] += A[mi+j];
		}
	}
}

int main() {

	int a_h[N], b_h[N];
	int (*a_d), (*b_d);

	int size = N*sizeof(int);

	//allocate the memory on the device
	cudaMalloc((void**)&a_d, size);
	cudaMalloc((void**)&b_d, size);

	//assign values to matrixes
	for(int i=0; i<N; i++)
	{
		a_h[i] = i;
		b_h[i] = 0;
	}



	//copy matrixes to the device
	cudaMemcpy(a_d, a_h, size, cudaMemcpyHostToDevice);
	cudaMemcpy(b_d, b_h, size, cudaMemcpyHostToDevice);

	// launch kernel on the device by combining two-dimensional blocks with two-dimensional threads
	//define numBlocks and threadsPerBlock
	dim3 numBlocks(1);
	dim3 numThreads(N);
	MatMul<<<numBlocks, numThreads>>>(a_d, b_d);

	//copy results from the device to the host
	cudaMemcpy(b_h, b_d, size, cudaMemcpyDeviceToHost);



	//print the results
	for(int i=0;i<N;i++){
        		printf("%d ", b_h[i]);
    		}	

	//free the memory
	cudaFree(a_d); 
	cudaFree(b_d); 

	return 0;
}
