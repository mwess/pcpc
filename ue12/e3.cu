#include "cuda.h"
#include "bitmap.h"

#define DIM 1024
#define PI 3.1415926535897932f

__global__ void kernel( unsigned char *ptr ) {
    // map from threadIdx/BlockIdx to pixel position
    int x = threadIdx.x + blockIdx.x * blockDim.x;
    int y = threadIdx.y + blockIdx.y * blockDim.y;
    int offset = x + y * blockDim.x * gridDim.x;

    __shared__ float  sub[16][16];

    // now calculate the value at that position
    const float period = 128.0f;

    sub[threadIdx.x][threadIdx.y] =
            255 * (sinf(x*2.0f*PI/ period) + 1.0f) *
                  (sinf(y*2.0f*PI/ period) + 1.0f) / 4.0f;
    __syncthreads();
	
	//assign color for each pixel position
    ptr[offset*4 + 0] = 0;
    ptr[offset*4 + 1] = sub[15-threadIdx.x][15-threadIdx.y];
    ptr[offset*4 + 2] = 0;
    ptr[offset*4 + 3] = 255;
}

// globals needed by the update routine
struct DataBlock {
    unsigned char   *dev_bitmap;
};

int main( void ) {

	//name a bitmap and assign values
    DataBlock   data;
    CPUBitmap bitmap( DIM, DIM, &data );
    unsigned char    *dev_bitmap;

    cudaMalloc( (void**)&dev_bitmap, bitmap.image_size());
    data.dev_bitmap = dev_bitmap;

    dim3    grids(DIM/16,DIM/16);
    dim3    threads(16,16);
    kernel<<<grids,threads>>>( dev_bitmap );

    cudaMemcpy( bitmap.get_ptr(), dev_bitmap, bitmap.image_size(), cudaMemcpyDeviceToHost );
                              
    cudaFree( dev_bitmap );
                              
    bitmap.display_and_exit();
}
