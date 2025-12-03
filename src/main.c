#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<getopt.h>
#include<ctype.h>
#include "file.h"
#include "parse.h"
#include "common.h"

void print_usage(char * argv[]) {
    printf("Usage: %s -f <file_path> [-n]\n", argv[0]);
    printf("  \t-n Create a new database file\n");
    printf("  \t-f <file_path>   Specify the path to the database file (required!)\n");

}

int main(int argc, char *argv[]) {
    char * file_path = NULL;
    bool newFile = false;
    int c;
    struct dbheader_t * dbHeader = NULL;
    int fd = -1;
    while ((c = getopt(argc, argv, "nf:")) != -1)
    {
        switch (c)
        {
        case 'n':
            newFile = true;
            break;
        case 'f':
            file_path = optarg;
            break;
        case '?':
            if (optopt == 'f')
                fprintf (stderr, "Option -%c requires an argument.\n", optopt);
            else if (isprint (optopt))
                fprintf (stderr, "Unknown option `-%c`.\n", optopt);
            else
                fprintf (stderr,
                         "Unknown option character `\\x%x`.\n",
                         optopt);
            return STATUS_ERROR;
        default:
            return STATUS_ERROR;
        }
    }

    if(file_path == NULL) {
        fprintf(stderr, "File path is required.\n");
        print_usage(argv);
        return STATUS_ERROR;
    }

    if (newFile) {
        fd = create_db_file(file_path);
        if(fd == STATUS_ERROR) {
            return STATUS_ERROR;
        }
        if(create_db_header(fd, &dbHeader) == STATUS_ERROR) {
            fprintf(stderr, "Failed to create database header\n");
            return STATUS_ERROR;
        }
    } else {
        fd = open_db_file(file_path);
        if(fd == STATUS_ERROR) {
            return STATUS_ERROR;
        }

        if(validate_db_header(fd, &dbHeader) == STATUS_ERROR) {
            fprintf(stderr, "Failed to validate database header\n");
            return STATUS_ERROR;
        }
        output_db_header(fd, dbHeader);
    }

    printf("File path: %s\n", file_path);
    printf("New file: %s\n", newFile ? "true" : "false");

    return STATUS_SUCCESS;
}