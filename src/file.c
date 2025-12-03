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

    int fd = open(filepath, O_RDWR | O_CREAT | O_TRUNC, 0644);
    if(fd < 0) {
        perror("open");
        return STATUS_ERROR;
    }

    return fd;
}

int open_db_file(const char * filepath) {
    if(filepath == NULL) {
        fprintf(stderr, "File path is NULL\n");
        return STATUS_ERROR;
    }

    int fd = open(filepath, O_RDWR);
    if(fd < 0) {
        perror("open");
        return STATUS_ERROR;
    }

    return fd;
}