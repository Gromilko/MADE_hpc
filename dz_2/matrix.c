#include <stdio.h>
#include <stdlib.h>
#include <time.h>

const size_t N = 1024;

void ZeroMatrix(double * A, size_t N)
{
	for(size_t i=0; i<N; i++)
	{
		for(size_t j=0; j<N; j++)
		{
			A[i*N+j] = 0.0;
		}
	}
}

void RandomMatrix(double * A, size_t N)
{
	srand(time(NULL));
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			A[i * N + j] = (double)rand() / (double)RAND_MAX;
		}
	}
}

int main()
{
	int NRuns = 5;
	size_t i, j, k;

	double *runtimes;
	double *A, *B, *C;

	A = (double *) malloc(N * N * sizeof(double));
	B = (double *) malloc(N * N * sizeof(double));
	C = (double *) malloc(N * N * sizeof(double));
	runtimes = (double *) malloc(N * N * sizeof(double));


	RandomMatrix(&A[0], N);
	RandomMatrix(&B[0], N);

	// for (i = 0; i < N; i++){
	// 	for(j = 0; j < N; j++){
	// 		printf("%lf ", A[i * N + j]);
	// 	}
	// 	printf("\n");
	// }

	// printf("B\n");
	// for (i = 0; i < N; i++){
	// 	for(j = 0; j < N; j++){
	// 		printf("%lf ", B[i * N + j]);
	// 	}
	// 	printf("\n");
	// }

	//=============================================
	struct timeval start, end;
    double r_time = 0.0;
    // size_t i, j, k;

    ZeroMatrix(&C[0], N);

    gettimeofday(&start, NULL);

	for (i = 0; i < N; i++){
		for(j = 0; j < N; j++){
			for(k = 0; k < N; k++){
				C[i * N + j] = C[i * N + j] + A[i * N + k] * B[k * N + j];
			}
		}
	}

	gettimeofday(&end, NULL);
    
    r_time = end.tv_sec - start.tv_sec + ((double) (end.tv_usec - start.tv_usec)) / 1000000;

    printf("time %lf ", r_time);

	//=============================================

	// printf("C\n");
	// for (i = 0; i < N; i++){
	// 	for(j = 0; j < N; j++){
	// 		printf("%lf ", C[i * N + j]);
	// 	}
	// 	printf("\n");
	// }

	free(A);
	free(B);
	free(C);

	free(runtimes);
	return 0;
}