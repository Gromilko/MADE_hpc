#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "matrix_functions.h"

size_t N = 512;



int main()
{
	int NRuns = 5;
	size_t i;

	double *runtimes;
	double *A, *B, *C;

	int Ns[4] = {512, 1024, 2048, 4096};

	for(i=0; i<4; i++)
    {
    	N = Ns[i];
    	
		A = (double *) malloc(N * N * sizeof(double));
		B = (double *) malloc(N * N * sizeof(double));
		C = (double *) malloc(N * N * sizeof(double));
		runtimes = (double *) malloc(N * N * sizeof(double));


		RandomMatrix(&A[0], N);
		RandomMatrix(&B[0], N);

    
    
    	double r_time = Matmul(&A[0], &B[0], &C[0], N);
    	printf("size: %d time: %lf \n", N, r_time);
    

		free(A);
		free(B);
		free(C);

		free(runtimes);
	}

	return 0;
}