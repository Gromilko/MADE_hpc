#include <stdlib.h>
#include <time.h>

void ZeroMatrix(double * A, int N)
{
	for(size_t i=0; i<N; i++)
	{
		for(size_t j=0; j<N; j++)
		{
			A[i*N+j] = 0.0;
		}
	}
}

void RandomMatrix(double * A, int N)
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

double Matmul(double * A, double * B, double * C, int N)
{
    struct timeval start, end;
    double r_time = 0.0;
    size_t i, j, k;

    ZeroMatrix(&C[0], N);

    gettimeofday(&start, NULL);
    
    for (i = 0; i < N; i++)
        for(j = 0; j < N; j++)
        {
            for(k = 0; k < N; k++)
                C[i * N + j] = C[i * N + j] + A[i * N + k] * B[k * N + j];
        }
    gettimeofday(&end, NULL);
    
    r_time = end.tv_sec - start.tv_sec + ((double) (end.tv_usec - start.tv_usec)) / 1000000;
    
    return r_time;
}