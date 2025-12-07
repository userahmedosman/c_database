#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<unistd.h>
#include<fcntl.h>
#include"file.h"
#include"common.h"

int create_db_file(const char * filepath) {
    if(filepath == NULL) {
        fprintf(stderr, "File path is NULL\n");
        return STATUS_ERROR;
    }

    // FIXED: Use the provided filepath, not hardcoded name
    // Added O_TRUNC to clear file if it already exists
    int fd = open(filepath, O_RDWR | O_CREAT | O_TRUNC, 0644);
    if(fd < 0) {
        perror("open");
        return STATUS_ERROR;
    }

    printf("Created database file: %s (fd=%d)\n", filepath, fd);
    return fd;
}

int open_db_file(const char * filepath) {
    if(filepath == NULL) {
        fprintf(stderr, "File path is NULL\n");
        return STATUS_ERROR;
    }

    // Added O_CREAT so file is created if missing
    int fd = open(filepath, O_RDWR | O_CREAT, 0644);
    if(fd < 0) {
        perror("open");
        return STATUS_ERROR;
    }

    printf("Opened database file: %s (fd=%d)\n", filepath, fd);
    return fd;
}