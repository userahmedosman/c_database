#include<stdio.h>
#include<stdlib.h>

#include "file.h"

FILE * open_file(const char * path, const char * mode) {
    FILE * file = fopen(path, mode);
    if(file == NULL) {
        fprintf(stderr, "Error opening file: %s\n", path);
        return NULL;
    }
    return file;
}