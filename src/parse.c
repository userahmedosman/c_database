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
#include<getopt.h>
#include<ctype.h>

int create_db_header(int fd, struct dbheader_t **headerOut) {
    if(fd < 0 || headerOut == NULL) {
        fprintf(stderr, "Invalid arguments to create_db_header\n");
        return STATUS_ERROR;
    }

    struct dbheader_t * header = calloc(1, sizeof(struct dbheader_t));
    if(header == NULL) {
        perror("calloc");
        return STATUS_ERROR;
    }

    header->magic = htonl(HEADER_MAGIC);
    header->version = htons(1);
    header->count = htons(0);
    header->filesize = htonl(sizeof(struct dbheader_t));

    ssize_t writtenBytes = write(fd, header, sizeof(struct dbheader_t));
    if(writtenBytes != sizeof(struct dbheader_t)) {
        perror("write");
        free(header);
        return STATUS_ERROR;
    }

    *headerOut = header;
    return STATUS_SUCCESS;
}

int read_employees(int fd, struct dbheader_t * header, struct employee_t ** employeesOut) {
    if(fd < 0 || header == NULL || employeesOut == NULL) {
        fprintf(stderr, "Invalid arguments to read_employees\n");
        return STATUS_ERROR;
    }

    unsigned short count = ntohs(header->count);
    if(count == 0) {
        *employeesOut = NULL;
        return STATUS_SUCCESS;
    }

    struct employee_t * employees = calloc(count, sizeof(struct employee_t));
    if(employees == NULL) {
        perror("calloc");
        return STATUS_ERROR;
    }

    ssize_t readBytes = read(fd, employees, count * sizeof(struct employee_t));
    for(int x = 0; x < count; x++) {
        employees[x].hours = ntohl(employees[x].hours);
    }
    if(readBytes != count * sizeof(struct employee_t)) {
        perror("read");
        printf("read expected to read %zu bytes, but read %zd bytes\n", count * sizeof(struct employee_t), readBytes);
        free(employees);
        return STATUS_ERROR;
    }

    *employeesOut = employees;
    return STATUS_SUCCESS;
}

int validate_db_header(int fd, struct dbheader_t **headerOut) {
    if(fd < 0) {
        fprintf(stderr, "Invalid file descriptor: %d\n", fd);
        return STATUS_ERROR;
    }

    // Check file size first
    struct stat st;
    if(fstat(fd, &st) == -1) {
        perror("fstat");
        return STATUS_ERROR;
    }
    
    if(st.st_size == 0) {
        fprintf(stderr, "File is empty (0 bytes)\n");
        return STATUS_ERROR;
    }
    
    if(st.st_size < sizeof(struct dbheader_t)) {
        fprintf(stderr, "File too small: %ld bytes, need %zu bytes\n", 
                st.st_size, sizeof(struct dbheader_t));
        return STATUS_ERROR;
    }

    struct dbheader_t * header = calloc(1, sizeof(struct dbheader_t));
    if(header == NULL) {
        perror("calloc");
        return STATUS_ERROR;
    }
    
    // Verify lseek worked
    off_t pos = lseek(fd, 0, SEEK_SET);
    if(pos == (off_t)-1) {
        perror("lseek");
        free(header);
        return STATUS_ERROR;
    }
    
    ssize_t readBytes = read(fd, header, sizeof(struct dbheader_t));
    
    if(readBytes == 0) {
        fprintf(stderr, "read() returned 0: EOF reached immediately\n");
        free(header);
        return STATUS_ERROR;
    }
    
    if(readBytes == -1) {
        perror("read failed");
        free(header);
        return STATUS_ERROR;
    }
    
    if(readBytes != sizeof(struct dbheader_t)) {
        fprintf(stderr, "Expected to read %zu bytes, but read %zd bytes\n", 
                sizeof(struct dbheader_t), readBytes);
        free(header);
        return STATUS_ERROR;
    }
    
    // CRITICAL: Convert network byte order
    header->version = ntohs(header->version);
    header->count = ntohs(header->count);
    header->filesize = ntohl(header->filesize);
    header->magic = ntohl(header->magic);

    // CRITICAL: Validate magic number and version
    if(header->version != 1){
        fprintf(stderr, "Invalid database file: unsupported version %u\n", header->version);
        free(header);
        return STATUS_ERROR;
    }
    
    if(header->magic != HEADER_MAGIC) {
        fprintf(stderr, "Invalid database file: incorrect magic number 0x%X (expected 0x%X)\n", 
                header->magic, HEADER_MAGIC);
        free(header);
        return STATUS_ERROR;
    }
    
    // CRITICAL: Validate file size
    if(header->filesize != (unsigned int)st.st_size) {
        fprintf(stderr, "Invalid database file: filesize mismatch (header: %u, actual: %ld)\n", 
                header->filesize, st.st_size);
        free(header);
        return STATUS_ERROR;
    }

    // CRITICAL: Set the output pointer
    *headerOut = header;
    return STATUS_SUCCESS;
}

int add_new_employee(struct dbheader_t * header,  struct employee_t * employees, char * employeeData) {


    // if(sscanf(employeeData, "%255[^,],%255[^,],%u", name, address, &hours) != 3) {
    //     fprintf(stderr, "Invalid employee data format. Expected format: name,address,hours\n");
    //     return STATUS_ERROR;
    // }
    printf("Adding new employee with data: %s\n", employeeData);
    char * name  = strtok(employeeData, ",");
    char * address = strtok(NULL, ",");
    char * hoursStr = strtok(NULL, ",");
    if(name == NULL || address == NULL || hoursStr == NULL) {
        fprintf(stderr, "Invalid employee data format. Expected format: name,address,hours\n");
        return STATUS_ERROR;
    }
    printf("Parsed Employee Data - Name: %s, Address: %s, Hours: %s\n", name, address, hoursStr);
  
    return STATUS_SUCCESS;
}
int output_db_header(int fd, struct dbheader_t * header) {
    if(fd < 0 || header == NULL) {
        fprintf(stderr, "Invalid arguments to output_db_header\n");
        return STATUS_ERROR;
    }

    // Convert to network byte order before writing
    header->count = htons(header->count);
    header->filesize = htonl(header->filesize);
    header->version = htons(header->version);
    header->magic = htonl(header->magic);
    
    lseek(fd, 0, SEEK_SET);

    if(write(fd, header, sizeof(struct dbheader_t)) != sizeof(struct dbheader_t)) {
        perror("write in output_db_header");
        return STATUS_ERROR;
    }

    // Convert back to host for display
    header->count = ntohs(header->count);
    header->filesize = ntohl(header->filesize);
    header->version = ntohs(header->version);
    header->magic = ntohl(header->magic);

    printf("Database Header:\n");
    printf(" Magic: 0x%X\n", header->magic);
    printf(" Version: %u\n", header->version);
    printf(" Record Count: %u\n", header->count);
    printf(" File Size: %u bytes\n", header->filesize);
    
    return STATUS_SUCCESS;
}