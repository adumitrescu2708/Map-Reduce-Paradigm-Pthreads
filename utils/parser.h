/*
    @author Dumitrescu Alexandra
    @since Nov 2022
*/

#ifndef _PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <string>
#include <vector>

#define MAX_CHARACTERS 50

using namespace std;

/*
    Parser.h
    
    | - Describes methods used for parsing the input
*/


void parse_input(int argc, char **argv, int *mappers, int *reducers, char **filenames);

void parse_mappers_files(char *filename, int *files_number, char ***files);

#endif