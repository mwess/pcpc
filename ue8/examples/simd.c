// compile with
// gcc -fopenmp-simd simd.c

// DO NOT use -fopenmp !!

#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>

#define MEGA    1000000UL
#define GIGA    1000000000UL
#define N       (100*MEGA)
#define RUNS    10

double          a[N], b[N], c[N];

int main() {
        long int        i, ops, j;
        double          s = 0, dt;
        struct timeval  tv0, tv1;

        for (i=0; i < N; i++) {
                a[i] = 5;
        }

        for (i=0; i < N; i++) {
                b[i] = N - 6;
        }

        gettimeofday(&tv0, NULL);

        for (j=0; j<RUNS; j++)

        #pragma omp parallel
        {
                #pragma omp for simd
                for (int i=0; i < N; i++) {
                        c[i] = (a[i]+b[i])*(a[i]-b[i]) + a[i]*a[i] + b[i]*b[i] + a[i] * b[i] + a[i] + b[i] + a[i]/(1+b[i]);
                }
        }

        gettimeofday(&tv1, NULL);

        dt = tv1.tv_sec - tv0.tv_sec + (tv1.tv_usec - tv0.tv_usec)/1000000.0;
        ops = 14*N*RUNS;
        printf("%.3lf Gflops (%ld ops in %.2lf s), result: %lf  %lf\n", ops/dt/GIGA, ops, dt, s, c[N-1]);

        exit(0);
}
