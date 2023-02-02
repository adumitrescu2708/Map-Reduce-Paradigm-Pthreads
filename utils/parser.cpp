/*
    @author Dumitrescu Alexandra
    @since Nov 2022
*/
#include "parser.h"

/* Method used for checking the inline command and for obtaining given parameters*/
void parse_input(int argc, char **argv, int *mappers, int *reducers, char **filename)
{
    if(argc < 4) {
        fprintf(stderr, "%s", "Wrong usage: ./tema1 <mappers> <reducers> <filename>");
        exit(1);
    }
    *mappers = atoi(argv[1]);
    *reducers = atoi(argv[2]);
    *filename = strdup(argv[3]);
}

/* Method used for parsing the list of files in the given file.
|  This list will then be used by mappers */
void parse_mappers_files(char *filename, int *files_number, char ***files)
{
    FILE *in = fopen(filename, "rt");
    if(in == NULL) {
        fprintf(stderr, "Can't open file %s", filename);
        exit(1);
    }

    fscanf(in, "%d", files_number);
    fscanf(in, "\n");
    
    
    (*files) = (char **) malloc((*files_number) * (MAX_CHARACTERS + 1));
    for(int i = 0; i < *files_number; i++) {
        (*files)[i] = (char *) malloc((MAX_CHARACTERS + 1) * sizeof(char));
    }

    for(int i = 0; i < *files_number; i++) {
        fgets((*files)[i], MAX_CHARACTERS, in);
        if((*files)[i][strlen((*files)[i]) - 1] == '\n') {
            (*files)[i][strlen((*files)[i]) - 1] = '\0';
        }
 
        (*files)[i][MAX_CHARACTERS] = '\0';
    }
    fclose(in);   
}