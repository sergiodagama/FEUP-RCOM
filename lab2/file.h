#ifndef FILE_H
#define FILE_H

typedef struct{
    char* name;
    unsigned char * data;
    unsigned long size;
} img_info;

int get_file_size(img_info* img, FILE * file);

int read_file(img_info* img, FILE *file);

#endif