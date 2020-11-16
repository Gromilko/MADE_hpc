#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h> 

const size_t MAX_NAME_LEN = 10; // у всех процессов имена prosessNN

int get_random_process(int prank, int psize)
{
    // вернуть рандомный номер процесса кроме собственного
    // return rand() % (max_number + 1 - minimum_number) + minimum_number
    int to_process = rand() % (psize);

    while (to_process == prank){
        to_process = rand() % (psize);;
    }
    return to_process;
}


int main(int argc, char ** argv)
{
    int prank, psize;
    MPI_Status status;
    int ierr;
    srand(time(NULL));
    

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &psize);
    MPI_Comm_rank(MPI_COMM_WORLD, &prank);


    int to_process;


    // будем отправлять и получать два мессива
    // имя процесса
    char *name;
    name = (char *) malloc(11*sizeof(char));

    // имена всех предыдущих процессов, храним в виде одномерного массива
    char *orderedNames;
    orderedNames = (char*)malloc(psize * MAX_NAME_LEN * sizeof(char));


    if(prank==0)
    {   
        // согласно условию, нулевой процесс должен отправить тлько имя
        to_process = get_random_process(prank, psize);
        printf("Send name from %d to %d\n", 0, to_process);
        MPI_Ssend("process0", MAX_NAME_LEN, MPI_UNSIGNED_CHAR, to_process, 0, MPI_COMM_WORLD);
    }
    else
    {
        // любой процесс кроме нулевого, должен получить имя от другого процесса
        MPI_Recv(&name[0], MAX_NAME_LEN, MPI_UNSIGNED_CHAR, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
        printf("Process %d received name %s from process with rank %d.\n", prank, name, status.MPI_SOURCE);

        // если сообщение принято от первого процесса это значит что было послано только имя, 
        // нам не нужно ждать сообщение с orderedNames, мы должны создать этот массив
        if(status.MPI_SOURCE == 0)
        {
            // memset(orderedNames, 0, psize * MAX_NAME_LEN * sizeof(char));
            for (int i = 0; i < psize * MAX_NAME_LEN; ++i)
                orderedNames[i] = 0;
            printf("Process %d initialize orderedNames ", prank); 
        }
        else
        {
            // если имя принято не от нулевого процесса, значит еще было передан массив с именами 
            MPI_Recv(&orderedNames[0], psize * MAX_NAME_LEN, MPI_CHAR, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
            printf("Process %d received orderedNames from process with rank %d ", prank, status.MPI_SOURCE);           
        }

        printf("and set %s in %d position\n", name, status.MPI_SOURCE);
        
        // добавим имя в массив
        for (int i = 0; i < sizeof(name)/sizeof(char)+1; i++)
            orderedNames[status.MPI_SOURCE*MAX_NAME_LEN + i] = name[i];

        // распечатаем массив с именами уже представившихся процесов
        for (int i = 0; i < psize*MAX_NAME_LEN; ++i)
        {
            if(orderedNames[i]==0)
                printf(".");
            else
                printf("%c", orderedNames[i]);
        }
        printf("\n");


        // посчитаем сколько процессов уже представилось 
        int count = 0;
        for (int i = 0; i < psize*MAX_NAME_LEN; i+=MAX_NAME_LEN)
        {
            if(orderedNames[i]!=0)
                count++;            
        }
        printf("Said 'hello' %d processes.\n", count);
        
        // если еще есть процессы которые не представились
        if (count+1 < psize)
        {
            // выбираем процесс который еще не представлялся
            to_process = get_random_process(prank, psize);
            while (orderedNames[to_process*MAX_NAME_LEN] !=0){
                to_process = get_random_process(prank, psize);
            }  
        
            // передаем ему имя текущего процесса
            char *name_own;
            name_own = (char *) malloc(11*sizeof(char));
            printf("Send name from %d to %d\n", prank, to_process);
            snprintf(name_own, MAX_NAME_LEN, "process%d", prank);
            
            MPI_Ssend(&name_own[0], MAX_NAME_LEN, MPI_CHAR, to_process, 0, MPI_COMM_WORLD);

            // и массив имен
            MPI_Ssend(&orderedNames[0], psize * MAX_NAME_LEN, MPI_CHAR, to_process, 0, MPI_COMM_WORLD);

        }
        // если все процессы предствились, печатаем из имена
        else
        {
            printf("STOP. Process %d knows the names of all other %d processes:\n", prank, psize-1);
            
            for (int i = 0; i < psize*MAX_NAME_LEN; i+=MAX_NAME_LEN)
            {
                if(orderedNames[i]==0)
                    continue;
                else
                {
                    printf("Process#%d name: ", (int)(i/MAX_NAME_LEN));
                    int j = 0;
                    while(orderedNames[i + j] != 0)
                    {
                        printf("%c", orderedNames[i + j]);
                        j++;
                    }
                    printf("\n");
                }
            }
        }
    }    
    printf("\n");
    MPI_Finalize();

    return 0;
}
