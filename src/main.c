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

    while ((c = getopt(argc, argv, "nf: ")) != -1)
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
            break;
        
        default:
            return -1;
        }
    }

    if(file_path == NULL) {
        fprintf(stderr, "File path is required.\n");
        print_usage(argv);
        return 0;
    }

    printf("File path: %s\n", file_path);
    printf("New file: %s\n", newFile ? "true" : "false");
    
    return 0;
}