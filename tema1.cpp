/*
    @author Dumitrescu Alexandra
    @since Nov 2022
*/
#include "utils/parser.h"
#include "utils/helper.h"
#include <iostream>
#include <limits.h>

using namespace std;

/*
    Main Class
    
    | - Responsible for parsing the input
    | - Initializes thread information and shared arrays (one for perfect powers
    |   and one for merged pperfect powers
    | - Starts and joins threads
    | - Frees allocated memory
*/

int main(int argc, char **argv)
{
    int mappers, reducers, mappers_files_number, number_threads, result;
    char *filename;
    char **mappers_files;

    /* Parse input - Obtain number of mappers and reducers and input filename */
    parse_input(argc, argv, &mappers, &reducers, &filename);
    /* Parse input - Obtain files for mappers */
    parse_mappers_files(filename, &mappers_files_number, &mappers_files);
    
    /* Initialize shared arrays for perfect powers and for merged perfect powers */
    vector<vector<vector<int>>>     perfect_powers(mappers, vector<vector<int>>(reducers + 2));
    vector<vector<int>>             merged_perfect_powers(reducers + 2);
    
    /* Compute number of threads */
    number_threads = mappers + reducers;

    /* Initialize threads array */
    pthread_t threads[number_threads];
    
    /* Shared mutex for threads */
    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);

    /* Shared barrier for threads */
    pthread_barrier_t barrier;
    pthread_barrier_init(&barrier, NULL, number_threads);

    /* For each thread initialize a thread data struct containing 
       shared information, mutex, barrier. */
    thread_data data[number_threads + 1];

    /* First the mappers are announced to process until there are no
       more files left to be processed. */
    int number_of_processed_files = 0;
    bool keep_processing = true;

    for(int i = 0; i < number_threads; i++) {
        /* Based on the local information, initialize the thread info */
        data[i] = init_thread_data(i,
                mappers,
                mappers_files_number,
                &number_of_processed_files,
                &keep_processing,
                mappers_files,
                reducers + 1,
                &perfect_powers,
                &merged_perfect_powers,
                &mutex, &barrier);
        
        /* Create and start thread and check for possible errors */
        result = pthread_create(&threads[i], NULL, thread_function, &data[i]);

        if(result) {
            fprintf(stderr, "Error in creating thread %i.\n", i);
            exit(1);           
        }
    }

    /* Join the threads and treat possible errors */
    for(int i = 0; i < number_threads; i++) {
        result = pthread_join(threads[i], NULL);
        if(result) {
            fprintf(stderr, "Error in waiting thread %i.\n", i);
            exit(1);  
        }
    }

    /* Destroy and free allocated memory */
    pthread_mutex_destroy(&mutex);
    pthread_barrier_destroy(&barrier);
    free(filename);
    for(int i = 0; i < mappers_files_number; i++) {
        free(mappers_files[i]);
    }
    free(mappers_files);

    return 0;
}