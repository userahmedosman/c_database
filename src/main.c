#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<getopt.h>
#include<ctype.h>
#include "file.h"
#include "parse.h"
#include "common.h"
#include <unistd.h>    // For lseek() and SEEK_SET
#include <sys/types.h> // For off_t type (return type of lseek)
void print_usage(char * argv[]) {
    printf("Usage: %s -f <file_path> [-n]\n", argv[0]);
    printf("  \t-n Create a new database file\n");
    printf("  \t-f <file_path>   Specify the path to the database file (required!)\n");

}

int main(int argc, char *argv[]) {
    char * file_path = NULL;
    char * add_employee = NULL;
    bool newFile = false;
    int c;
    struct dbheader_t * dbHeader = NULL;
    struct employee_t * employee = NULL;
    int fd = -1;
    
    while ((c = getopt(argc, argv, "nf:a:")) != -1) {
        switch (c) {
        case 'n':
            newFile = true;
            break;
        case 'f':
            file_path = optarg;
            break;
        case 'a':
            add_employee = optarg;
            break;    
        case '?':
            if (optopt == 'f')
                fprintf (stderr, "Option -%c requires an argument.\n", optopt);
            else if (isprint (optopt))
                fprintf (stderr, "Unknown option `-%c`.\n", optopt);
            else
                fprintf (stderr, "Unknown option character `\\x%x`.\n", optopt);
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
        
        // CRITICAL FIX: After creating header, we need to read it back
        // Reset file position and validate what we just wrote
        
        if(validate_db_header(fd, &dbHeader) == STATUS_ERROR) {
            fprintf(stderr, "Failed to validate newly created database header\n");
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

        if(read_employees(fd, dbHeader, &employee) == STATUS_ERROR) {
            fprintf(stderr, "Failed to read employees\n");
            return STATUS_ERROR;
        }
    }
    
    // CRITICAL FIX: Handle employee addition for BOTH new and existing files
    if(add_employee != NULL) {
        printf("Adding new employee: %s\n", add_employee);
        if(add_new_employee(dbHeader, employee, add_employee) == STATUS_ERROR) {
            fprintf(stderr, "Failed to add new employee\n");
            return STATUS_ERROR;
        }
    }
    
    // Update and output header (for both cases)
    if(output_db_header(fd, dbHeader) == STATUS_ERROR) {
        fprintf(stderr, "Failed to output database header\n");
        return STATUS_ERROR;
    }
    
    printf("File path: %s\n", file_path);
    printf("New file: %s\n", newFile ? "true" : "false");

    // Cleanup
    if(dbHeader) free(dbHeader);
    if(employee) free(employee);

    
    return STATUS_SUCCESS;
}