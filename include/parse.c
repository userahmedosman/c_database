#include<stdio.h>
#include<stdlib.h>

void create_file(const char *file_path) {
    FILE *file = fopen(file_path, "w");
    if (file == NULL) {
        perror("Error creating file");
        exit(EXIT_FAILURE);
    }
    fclose(file);
}