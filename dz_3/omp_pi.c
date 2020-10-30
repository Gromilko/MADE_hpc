#include <stdio.h>        // printf()
#include <stdlib.h>       // srand() and rand()
#include <time.h>        // time()

#include <omp.h>         // OpenMP functions and pragmas

/***  OMP ***/
#define nThreads 1  // number of threads to use
unsigned int seeds[nThreads];

void seedThreads() {
    int my_thread_id;
    unsigned int seed;
    #pragma omp parallel private (seed, my_thread_id)
    {
        my_thread_id = omp_get_thread_num();
        
        //create seed on thread using current time
        unsigned int seed = (unsigned) time(NULL);
        
        //munge the seed using our thread number so that each thread has its
        //own unique seed, therefore ensuring it will generate a different set of numbers
        seeds[my_thread_id] = (seed & 0xFFFFFFF0) | (my_thread_id + 1);
        
        printf("Thread %d has seed %u\n", my_thread_id, seeds[my_thread_id]);
    }
    
}


int main()
{
    omp_set_num_threads(nThreads);  
    seedThreads(); 

    int numFlips,           //loop control
        in, out; //counters

    double ompStartTime, ompStopTime;   
    int tid;       // thread id when forking threads in for loop
    unsigned int seed;   // seed each thread will use in for loop

    unsigned int trialFlips = 1000000000;          // start with a smal number of flips

    struct timeval start, end;
    double r_time = 0.0;

    gettimeofday(&start, NULL);
    
    #pragma omp parallel num_threads(nThreads) default(none) \
        private(numFlips, tid, seed) \
        shared(trialFlips, seeds) \
        reduction(+:in, out)
    {
        tid = omp_get_thread_num();   // my thread id
        seed = seeds[tid];            // it is much faster to keep a private copy of our seed
        srand(seed);                  //seed rand_r or rand
        
        double x, y;

        #pragma omp for
        for (numFlips=0; numFlips<trialFlips; numFlips++) {
            // in Windows, can use rand()
            // if (rand()%2 == 0) // if random number is even, call it heads
            // linux: rand_r() is thread safe, to be run on separate threads concurrently

            x = (rand_r(&seed)/(double)RAND_MAX);
            y = (rand_r(&seed)/(double)RAND_MAX);

            

            if ((x-0.5)*(x-0.5) + (y-0.5)*(y-0.5) < 0.25){
                in++;
            }
        }
        
    }


    gettimeofday(&end, NULL);    
    r_time = end.tv_sec - start.tv_sec + ((double) (end.tv_usec - start.tv_usec)) / 1000000;


    printf("trialFlips= %lu\n", trialFlips);
    printf("in %d\n", in);
    printf("pi= %f\n", ((double)in/(double)trialFlips)*4.);
    printf("nThreads: %d, time: %lf \n", nThreads, r_time);
    

}
