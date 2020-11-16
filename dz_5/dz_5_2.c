#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int FIELD_SIZE = 12000;
const size_t STEPS = 60000;

const size_t DEBUG = 0;
const size_t SHOW = 0;


void init_field(int *field, int size, int prank, int psize)
{
	for (int i = 0; i < size; ++i)
    	field[i] = rand() % 2;
  //   for (int i = 0; i < size; ++i)
  //   	field[i] = 0;
  //   if(prank == 1)
		// field[1] = 1;
}


void print_field(int *field, int size)
{
	for (int i = 0; i < size; ++i)
	{
		if(!DEBUG)
		{
			if(field[i] == 1)
				printf("%s", "■");
			else
				printf("%s", "□");
		}
    	else 
    		printf("%d ", field[i]);
	}
    printf("\n");
}

int rule101(int a, int b, int c)
	{
		if (a == 1 && b == 1 && c == 1)
			return(0);
		else if (a == 1 && b == 1 && c == 0)
			return(1);
		else if (a == 1 && b == 0 && c == 1)
			return(1);
		else if (a == 1 && b == 0 && c == 0)
			return(0);
		else if (a == 0 && b == 1 && c == 1)
			return(1);
		else if (a == 0 && b == 1 && c == 0)
			return(1);
		else if (a == 0 && b == 0 && c == 1)
			return(1);
		else if (a == 0 && b == 0 && c == 0)
			return(0);
	}

int rule161(int a, int b, int c)
{
	if (a == 1 && b == 1 && c == 1)
			return(1);
		else if (a == 1 && b == 1 && c == 0)
			return(0);
		else if (a == 1 && b == 0 && c == 1)
			return(1);
		else if (a == 1 && b == 0 && c == 0)
			return(0);
		else if (a == 0 && b == 1 && c == 1)
			return(0);
		else if (a == 0 && b == 1 && c == 0)
			return(0);
		else if (a == 0 && b == 0 && c == 1)
			return(0);
		else if (a == 0 && b == 0 && c == 0)
			return(1);
}

void rule_step(int *field, int *field_tmp, int size, int rule_id)
{
	int a, b, c;

	for (int i = 1; i < size-1; ++i)
	{	
		a = field[i-1];
		b = field[i];
		c = field[i+1];

		if(rule_id == 101)
			field_tmp[i] = rule101(a, b, c);
		else if(rule_id == 161)
			field_tmp[i] = rule161(a, b, c);
		else
			printf("ERROR! Unknown rule id %d.\n", rule_id);
	}
}

int main(int argc, char ** argv)
{
	int prank, psize;
	MPI_Status status;
	MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &psize);
    MPI_Comm_rank(MPI_COMM_WORLD, &prank);

	srand(time(NULL) + prank);
	
	int FIELD_SIZE_local = FIELD_SIZE;

	// органичение этой реализации - поле должно делиться между процессами на одинаковые части
	if(FIELD_SIZE % psize != 0)
	{
		if(prank == 0)
		{	
			printf("WARNING! The field of length %d cannot be divided equally between %d processes. ", FIELD_SIZE, psize);
			printf("Change field length or number of processes\n");
		}
		MPI_Finalize();
		return 0;
	}
	else
		FIELD_SIZE_local = (int)(FIELD_SIZE / psize);


    int *chunk;
    // +2 на клетки призраки
    chunk = (int *) malloc((FIELD_SIZE_local+2)*sizeof(int));
    init_field(chunk, FIELD_SIZE_local+2, prank, psize);

	int *chunk_next;
    chunk_next = (int *) malloc((FIELD_SIZE_local+2)*sizeof(int));
    for (int i = 0; i < FIELD_SIZE_local+2; ++i)
    	chunk_next[i] = 0;

    

    int *field;
    field = (int *) malloc(FIELD_SIZE*sizeof(int));
    for (int i = 0; i < FIELD_SIZE; ++i)
    	field[i] = 0;

    int *tmp_chunk;
    tmp_chunk = (int *) malloc((FIELD_SIZE_local)*sizeof(int));
	
	// for(int n=0; n<5; n++)
	// {
		double time_elapsed = MPI_Wtime();
		for (int step = 0; step < STEPS; ++step)
		{
			for (int i = 0; i < FIELD_SIZE; ++i)
		    	field[i] = 0;

		    // 1. Выполняем обмен клетками-призраками
		    // для двух процессов обмен клетками-призраками немного отличается от остальных случаев
		    if(psize == 2)
		    {
		    	if (prank == 0)
		    	{
		    		MPI_Send(&chunk[1], 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
		    		MPI_Send(&chunk[FIELD_SIZE_local], 1, MPI_INT, 1, 0, MPI_COMM_WORLD);

		    		MPI_Recv(&chunk[FIELD_SIZE_local+1], 1, MPI_INT, 1, 0, MPI_COMM_WORLD, &status);
		    		MPI_Recv(&chunk[0], 1, MPI_INT, 1, 0, MPI_COMM_WORLD, &status);
		    	}

		    	if (prank == 1)
		    	{
		    		MPI_Recv(&chunk[FIELD_SIZE_local+1], 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
		    		MPI_Recv(&chunk[0], 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

		    		MPI_Send(&chunk[1], 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
		    		MPI_Send(&chunk[FIELD_SIZE_local], 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
		    	}
		    }
		    else if(psize > 2)
		    {
		    	//процесс с которым обменяемся ghost cell
			    if(prank == 0)
			    {
			    	MPI_Send(&chunk[1], 1, MPI_INT, psize-1, 0, MPI_COMM_WORLD);
					// printf("Process %d send first ghost cell %d to process with rank %d.\n", prank, chunk[1], psize-1);
				}
			    else
			    	MPI_Send(&chunk[1], 1, MPI_INT, prank-1, 0, MPI_COMM_WORLD);
				

				if(prank == psize-1)
					MPI_Send(&chunk[FIELD_SIZE_local], 1, MPI_INT, 0, 0, MPI_COMM_WORLD); // FIELD_SIZE_local - 1 + 1
				else
					MPI_Send(&chunk[FIELD_SIZE_local], 1, MPI_INT, prank+1, 0, MPI_COMM_WORLD); // FIELD_SIZE_local - 1 + 1


				if(prank == 0)
			    	MPI_Recv(&chunk[0], 1, MPI_INT, psize-1, 0, MPI_COMM_WORLD, &status);
			    else
			    	MPI_Recv(&chunk[0], 1, MPI_INT, prank-1, 0, MPI_COMM_WORLD, &status);

			    if(prank == psize-1)
			    	MPI_Recv(&chunk[FIELD_SIZE_local+1], 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
			    else
			    	MPI_Recv(&chunk[FIELD_SIZE_local+1], 1, MPI_INT, prank+1, 0, MPI_COMM_WORLD, &status);
		    }

		    // 2. Делаем один шаг по правилу. Доступны правила 101 и 161
	    	rule_step(chunk, chunk_next, FIELD_SIZE_local+2, 161);
	    	
	    	int *tmp;
	    	tmp = chunk;
	    	chunk = chunk_next;
	    	chunk_next = tmp;


	    	if(SHOW)
	    	{
	    		// чтобы сделать визуализацию на каждом шаге, нулевой процесс будет собирать 
		    	// локальные результаты на каждом процессе и печатать общий результат
		    	if(prank == 0)
		    	{
		    		for (int i = 0; i < FIELD_SIZE_local; ++i)
			    		field[i] = chunk[i+1];

			    	for (int p_count = 1; p_count < psize; ++p_count)
			    	{
				    	MPI_Recv(&tmp_chunk[0], FIELD_SIZE_local, MPI_INT, p_count, 0, MPI_COMM_WORLD, &status);
			        	// printf("Process %d received data from process with rank %d.\n", prank, status.MPI_SOURCE);
			        	// print_field(tmp_chunk, FIELD_SIZE_local);

				    	for (int i = 0; i < FIELD_SIZE_local; ++i)
				    		field[status.MPI_SOURCE*FIELD_SIZE_local + i] = tmp_chunk[i];
				    }
				    
				    print_field(field, FIELD_SIZE);
		    	}
		    	else
					MPI_Send(&chunk[1], FIELD_SIZE_local, MPI_INT, 0, 0, MPI_COMM_WORLD);
		    }
		}

		time_elapsed = MPI_Wtime() - time_elapsed;
		if(prank==0)
			printf("%f\n", time_elapsed);	
	// }
    
	
    MPI_Finalize();
	return 0;	
}

