#ifndef FILE_H
#define FILE_H

#include <stdio.h>
#include <sys/stat.h>

typedef struct{
    char* name;
    unsigned char * data;
    unsigned long size;
} FileInfo;

int get_file_size(FileInfo* fi, FILE * file);

int read_file(FileInfo* fi, FILE *file);

int fillInfo(FileInfo* fi, FILE *file, char *file_name);

unsigned char* dataChunk(unsigned char* data, int start_index, int quantity);

#endif