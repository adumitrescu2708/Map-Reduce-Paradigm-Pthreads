/*
    @author Dumitrescu Alexandra
    @since Nov 2022
*/
#include "helper.h"
#include "parser.h"
#include "perfect_power.h"
#include <iostream>
#include <algorithm>
#include <string.h>

using namespace std;

/* Helper.cpp

    | - Implements described logic in @helper.h
    
*/


/*  In order to make sure that threads use the same shared data
|   for perfect powers found by mappers and merged powers
|   we use this method for creating the thread data
|   with data allocated in main */
struct thread_data init_thread_data(int thread_id,
            int     mappers_number,
            int     number_of_files,
            int     *number_of_processed_files,
            bool    *keep_processing,
            char    **mappers_files,
            int     max_power,
            vector<vector<vector<int>>>     *perfect_powers,
            vector<vector<int>>             *merged,
            pthread_mutex_t                 *mutex,
            pthread_barrier_t               *barrier)
{
    struct thread_data data;

    data.thread_id = thread_id;
    data.mappers_number = mappers_number;
    data.number_of_files = number_of_files;
    data.number_of_processed_files = number_of_processed_files;
    data.keep_processing = keep_processing;
    data.mappers_files = mappers_files;
    data.max_power = max_power;
    data.mutex = mutex;
    data.barrier = barrier;
    data.perfect_powers = perfect_powers;
    data.merged = merged;
    
    return data;
}

/*  Main thread function
| Based on the index from the incoming thread data we decide wether
| a thread is going to be a mapper or a reducer.
|
| In the main function we simultaneously start all P threads, but we
| keep track of the total number of mappers. This way
|   thread_id  in   0:number_of_mappers => thread will be a mapper
|   thread_id  in   number_of_mappers:P => thread will be a reducer
|
| Mappers:
|   -> Use the shared counter for total number of processed files
|      to decide what file to process next.
|   -> After taking the file, increment the counter                 (1)
|   -> Use the shared mutex to avoid race condition                 (2)
|   -> Process the file                                             (4)
|   -> If ready, take the next available one
|   -> If the counter if equal with the total number available      (3)
|      announce reducers to stop processing using the shared boolean value
|
| Reducers:
|   -> Based on the previous formula, compute the corresponding exponent
|      for each reducer.                                            (5)
|   -> Collect from each mapper all numbers perfect powers of the exponent
|      and create a list with them                                  (6)
|   -> Sort the list in ascending order                             (7)
|   -> Compute the number of different numbers in the previous list (8)
|   -> Create and write result in the corresponding output file     (9)
*/
void *thread_function(void *arg)
{
    struct thread_data data = *(struct thread_data *) arg;
    int file_to_process;

    /* Mappers part */
    if(data.thread_id < data.mappers_number) {
        while(*data.keep_processing == true) {
            /* (2) */
            pthread_mutex_lock(data.mutex);
            
            /* (1) */
            file_to_process = *data.number_of_processed_files;
            *data.number_of_processed_files = *data.number_of_processed_files + 1;
            
            /* (3) */
            if(*data.number_of_processed_files == (data.number_of_files + 1)) {
                *data.keep_processing = false;
            }
            pthread_mutex_unlock(data.mutex);
            
            /* (4) */
            if(*data.keep_processing == false) {
                break;
            }
            process_file(data.mappers_files[file_to_process], arg);
        }
    }

    /* Wait for all mappers to finish processing */
    pthread_barrier_wait(data.barrier);

    if(data.thread_id >= data.mappers_number) {
        /* (5) */
        int exponent = data.thread_id - data.mappers_number + 2;

        /* (6) */
        for(int mapper_id  = 0; mapper_id < data.mappers_number; mapper_id++) {
            for(int i = 0; i < (int) (*data.perfect_powers)[mapper_id][exponent].size(); i++) {
                (*data.merged)[exponent].push_back((*data.perfect_powers)[mapper_id][exponent][i]);
            }     
        }

        /* (7) */
        sort((*data.merged)[exponent].begin(), (*data.merged)[exponent].end());

        /* (8) */
        int different_number = 1;
        int copy;
        if((*data.merged)[exponent].size() > 0) {
            copy = (*data.merged)[exponent][0];
        } else {
            copy = 0;
        }

        /* Again (8) */
        for(int i = 1; i < (int) (*data.merged)[exponent].size(); i++) {
            if((*data.merged)[exponent][i] != copy) {
                copy = (*data.merged)[exponent][i];
                different_number ++;
            }
        }
        
        /* (9) - Compute output filename & path */
        char output_file[50];
        output_file[0] = '\0';
        strcat(output_file, "out");
        
        output_file[3] = (char) (exponent + (int) '0');
        output_file[4] = '\0';
        strcat(output_file, ".txt");

        /* (9) - Create file */
        FILE *out = fopen(output_file, "wt");
        if(out == NULL) {
            fprintf(stderr, "Can't open file %s", output_file);
            exit(1);                
        }
        /* (9) - Write result */
        fprintf(out, "%d", different_number);
        fclose(out);

    }
    
    pthread_exit(NULL);
}

/*  Process File function used by mappers
|   -> Compute the path to the given filename       (1)
|   -> Open the file and treat possible errors      (2)
|   -> For each number found in the file, check if it's a perfect power    (3)
*/
void process_file(char *filename, void *arg)
{
    /* (1) */
    char filepath[MAX_CHARACTERS];
    filepath[0] = '\0';
    strcat(filepath, "./");
    filepath[2] = '\0';
    strcat(filepath, filename);
    filepath[2 + strlen(filename)] = '\0';

    struct thread_data *data = (struct thread_data *) arg;

    /* (2) */
    FILE *in = fopen(filepath, "rt");
        if(in == NULL) {
        fprintf(stderr, "Can't open file %s", filepath);
        
        exit(1);
    }

    /* (3) */
    int n, x;
    fscanf(in, "%d", &n);
    for(int i = 0; i < n; i++) {
        fscanf(in, "%d", &x);
        check_perfect_power(x, data->max_power,
                    data->perfect_powers,
                    data->thread_id);
    }

    fclose(in);
}