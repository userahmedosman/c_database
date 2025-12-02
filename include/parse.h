#ifndef PARSE_H
#define PARSE_H


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

char * parse_file_path(int argc, char *argv[]);

bool parse_new_file_flag(int argc, char *argv[]);

#endif // PARSE_H