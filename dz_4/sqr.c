#include<stdio.h>
#include<stdlib.h>
#include <time.h>
#include <omp.h>

#include <time.h>
void RandomMatrix(int * A, int N)
{
	srand(time(NULL));
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			double r = (double)rand() / (double)RAND_MAX;
			A[i * N + j] = (int)(r*100.);
		}
	}
}

int* matpower2(int *Adj_m, int N)
{
	int* result = (int *)malloc(sizeof(int) * N * N);
	int i, j, k;

	#pragma omp parallel default(none) shared(result, N, Adj_m)
	{
		#pragma omp for private(i, j)
		for (i = 0; i < N; i++)
			for (j = 0; j < N; j++)
				result[i * N + j] = 0;


		#pragma omp for private(i, j, k)
		for (i = 0; i < N; i++)
			for (j = 0; j < N; j++)
				for (k = 0; k < N; k++)
					result[i * N + j] += Adj_m[i * N + k] * Adj_m[k * N + j];
	}
	return result;
}

int* matmul(int * A, int * B, int N)
{
    size_t i, j, k;

	int* C = (int *) malloc(N * N * sizeof(int));
	

	#pragma omp parallel default(none) shared(A, B, C, N)
	{
		#pragma omp for private(i, j)
		for(i=0; i<N; i++)
			for(j=0; j<N; j++)
				C[i*N+j] = 0;

		#pragma omp for private(i, j, k)
		for (i = 0; i < N; i++)
	    	for(j = 0; j < N; j++)
	        	for(k = 0; k < N; k++)
	            	C[i * N + j] = C[i * N + j] + A[i * N + k] * B[k * N + j];
	}

    return C;
}


int*  matbinpower(int *mtr, int N, int power) {
	
	int* result = (int *)malloc(sizeof(int) * N * N);
	int i, j;

	#pragma omp parallel for default(none) private(i, j) shared(N, result)
	for(i=0; i<N; i++)
	{
		for(j=0; j<N; j++)
		{	
			if (i!=j)
				result[i*N+j] = 0;
			else
				result[i*N+j] = 1;
		}
	}
	

	while (power) {
		// if (power % 2 != 0)
		if (power & 1)
			result = matmul(result, mtr, N);

		mtr = matpower2(mtr, N);
		// power /=2;
		power >>= 1;
	}

	return result;
}




int main(int argc, char const *argv[])
{	

	// граф-кольцо или рандомный
	// bool circle = true;
	int circle = 1;

	int i, j;
	int N = 1024;


	int *Adj_m = (int *)malloc(sizeof(int) * N * N);

	double start, end;
	start = omp_get_wtime();


	// создать граф-кольцо или рандомный
	if (circle)
	{
		#pragma omp parallel default(none) shared(N, Adj_m)
		{
			// заполнить матрицу нулями. Здесь и далее не использую отдельную функцию,
			// чтобы не выходить из уже имеющейся параллельной секции и не создавать новую 
			#pragma omp for private(i, j)
			for(i=0; i<N; i++)
				for(j=0; j<N; j++)
					Adj_m[i*N+j] = 0;

			#pragma omp for private(i)
			for (i = 1; i < N-1; i++)
			{
				Adj_m[i * N + i + 1] = 1;
				Adj_m[i * N + i - 1] = 1;
			}
		}

		// 0
		Adj_m[0 * N + 1] = 1;
		Adj_m[0 * N + N - 1] = 1;

		// N
		Adj_m[(N-1) * N + 0] = 1;
		Adj_m[(N-1) * N + N - 2] = 1;
	}
	else
	{
		RandomMatrix(&Adj_m[0], N);	
	}
	

	// печать матрицы
	// for (i = 0; i < N; i++)
	// {
	// 	for (j = 0; j < N; j++)
	// 		printf("%d ", Adj_m[N * i + j]);
	// 	printf("\n");
	// }
	// printf("\n\n");

	// посчитаем количество путей
	int *matsq;
	matsq = matbinpower(Adj_m, N, 33);


	// for (i = 0; i < N; i++)
	// {
	// 	for (j = 0; j < N; j++)
	// 		printf("%d ", matsq[N * i + j]);
	// 	printf("\n");
	// }

	end = omp_get_wtime();
    printf("size: %d time total: %lf \n", N, end-start);

	free(Adj_m);
	free(matsq);

	return 0;
}
