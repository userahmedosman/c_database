#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include"parse.h"
#include"common.h"

int create_db_header(int fd, struct dbheader_t **headerOut) {
    struct dbheader_t * header = calloc(1, sizeof(struct dbheader_t));
    if(header == NULL) {
        perror("calloc");
    return STATUS_ERROR;
    }

    header->magic = htonl(HEADER_MAGIC);
    header->version = htons(1);
    header->count = htons(0);
    header->filesize = htonl(sizeof(struct dbheader_t));

    

    *headerOut = header;
    return STATUS_SUCCESS;
}

int validate_db_header(int fd, struct dbheader_t **headerOut) {
    if(fd < 0) {
        fprintf(stderr, "Invalid file descriptor\n");
        return STATUS_ERROR;
    }

    struct dbheader_t * header = calloc(1, sizeof(struct dbheader_t));
    if(header == NULL) {
        perror("calloc");
        return STATUS_ERROR;
    }

    ssize_t readBytes = read(fd, header, sizeof(struct dbheader_t));
    if(readBytes != sizeof(struct dbheader_t)) {
        perror("read");
        printf("Expected to read %zu bytes, but read %zd bytes\n", sizeof(struct dbheader_t), readBytes);
        free(header);
        return STATUS_ERROR;
    }
    header->version = ntohs(header->version);
    header->count = ntohs(header->count);
    header->filesize = ntohl(header->filesize);
    header->magic = ntohl(header->magic);

    if(header->version != 1){
        fprintf(stderr, "Invalid database file: unsupported version %u\n", header->version);
        free(header);
        return STATUS_ERROR;
    }
    if(header->magic != HEADER_MAGIC) {
        fprintf(stderr, "Invalid database file: incorrect magic number\n");
        free(header);
        return STATUS_ERROR;
    }
    struct stat st;
    if(fstat(fd, &st) == -1) {
        perror("fstat");
        free(header);
        return STATUS_ERROR;
    }
    if(header->filesize != (unsigned int)st.st_size) {
        fprintf(stderr, "Invalid database file: filesize mismatch\n");
        free(header);
        return STATUS_ERROR;
    }

    *headerOut = header;
    return STATUS_SUCCESS;
}
void output_db_header(int fd, struct dbheader_t * header) {
    if(header == NULL) {
        fprintf(stderr, "Header is NULL\n");
        return;
    }

    printf("Database Header:\n");
    printf(" Magic: 0x%X\n", header->magic);
    printf(" Version: %u\n", header->version);
    printf(" Record Count: %u\n", header->count);
    printf(" File Size: %u bytes\n", header->filesize);
}