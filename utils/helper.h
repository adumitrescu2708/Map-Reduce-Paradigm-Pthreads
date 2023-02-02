/*
    @author Dumitrescu Alexandra
    @since Nov 2022
*/

#ifndef _HELPER_H
#include <vector>
#include <unordered_map>
using namespace std;

/*
    Helper.h
    
    | - Describes main methods used by mappers and reducers
    | - Describes the thread data
*/

struct thread_data {
    /*  Id of a thread & total number of threads
    |   Depending on the id of a thread and number of mappers
    |   we will decide wether a thread is a mapper or a reducer */

    int         thread_id;
    int         mappers_number;

    /* Data used by mappers for file processing 
    |  In order to decide for each mapper thread what file to process
    |  we will keep a shared boolean that is responsible for announcing
    |  all mappers when to stop processing, a counter incremented for
    |  each processed file and a shared list of files to process. */
    int         number_of_files;
    int         *number_of_processed_files;
    bool        *keep_processing;
    char        **mappers_files;

    /* Data used by reducers */
    int         max_power;

    /* Shared arrays for perfect powers found by mappers
       and merged perfect powers array for reducers.

       perfect_powers[i][j][k] = mapper with id i
       merged[i][j] = all numbers in the given form: x^i 

    */
    vector<vector<vector<int>>>             *perfect_powers;
    vector<vector<int>>                     *merged;

    /* Shared mutex and barrier */
    pthread_mutex_t                         *mutex;
    pthread_barrier_t                       *barrier;    
};

/* Init thread data method with given parameters */
struct thread_data init_thread_data(int     thread_id,
            int                             mappers_number,
            int                             number_of_files,
            int                             *number_of_processed_files,
            bool                            *keep_processing,
            char                            **mappers_files,
            int                             max_power,
            vector<vector<vector<int>>>     *perfect_powers,
            vector<vector<int>>             *merged,
            pthread_mutex_t                 *mutex,
            pthread_barrier_t               *barrier);

/* Thread function used by mappers and reducers */
void *thread_function(void *arg);

/* Separate function for processing file used by mappers */
void process_file(char *filename, void *arg);


#endif