#include <stdio.h>

#define DIM 100

struct point {
	float   r;
	float   i;
	//construct function
	point( float a, float b )  {r=a; i=b;}

	float magnitude2() { return r * r + i * i; }
	
	//override the operator *
    	point operator*(const point& a) {
        	return point(r*a.r - i*a.i, i*a.r + r*a.i);
    	}

	//override the operator +
    	point operator+(const point& a) {
        	return point(r+a.r, i+a.i);
    	}
};


	//according to the position to do calculation. if the value is bigger than 1000, return 0; otherwise, return 1.
int calculate( int x, int y ) { 
	

    	const float scale = 1.5;
	float jx = scale * (float)(DIM/2 - x)/(DIM/2);
    	float jy = scale * (float)(DIM/2 - y)/(DIM/2);

    	point c(-0.8, 0.156);
    	point a(jx, jy);

    	int i = 0;
    	for (i=0; i<200; i++) {
        	a = a * a + c;
        	if (a.magnitude2() > 1000)
            		return 0;
    	}

    	return 1;
}

//render each point of the image
void render( int image[DIM][DIM] ){
   	for (int y=0; y<DIM; y++) {
        for (int x=0; x<DIM; x++) {
            int value = calculate( x, y );
            image[x][y] = 255 * value;
        }
    	}
 }

int main() {
   	int image[DIM][DIM];

	for (int y=0; y<DIM; y++) {
        for (int x=0; x<DIM; x++) {
		image[x][y]=30;
	}
	}
    	render(image);

	for (int y=0; y<DIM; y++) {
        for (int x=0; x<DIM; x++) {
		printf("%d ",image[x][y]);
	}
	printf("\n");
	}
}
