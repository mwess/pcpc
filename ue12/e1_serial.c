#include <stdio.h>

#define DIM 20
#define TURNS 1

int main(){
	
	float grid[DIM][DIM];
	float tmp = 0;

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

	for(int k = 0; k < TURNS; k++){
		for(int i = 0; i < DIM; i++){
			for(int j = 0; j < DIM; j++){
				tmp = 0;
				for(int l = i-1; l <= i + 1; l++){
					for(int m = j-1; m <= j+1; m++){
						if(!(m < 0 || l < 0 || m >= DIM || l >= DIM)){
							tmp += grid[l][m];
						}
					}
				}
				grid[i][j] = tmp/9;
			}
		}
	}

	for(int i = 0; i < DIM; i++){
		for(int j = 0; j < DIM; j++){
			printf("%f ", grid[i][j]);
		}
		printf("\n");
	}
	return 0;
}
