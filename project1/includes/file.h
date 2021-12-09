

/**
 * @brief file related function
 */


#ifndef FILE_H
#define FILE_H

#include <stdio.h>
#include <sys/stat.h>

/**
 * @brief saves the file's information
 */

typedef struct{
    char* name;
    unsigned char * data;
    unsigned long size;
} FileInfo;

/**
 * @brief gets the file's size
 * 
 * @param fi file's information struct
 * @param file  file
 * 
 * @return 0 if successful, -1 otherwise
 */

int get_file_size(FileInfo* fi, FILE * file);

/**
 * @brief gets the file's data
 * 
 * @param fi file's information struct
 * @param file  file
 * 
 * @return 0 if successful, -1 otherwise
 */
int read_file(FileInfo* fi, FILE *file);

/**
 * @brief gets the file's size
 * 
 * @param fi file's information struct
 * @param file  file
 * 
 * @return 0 if successful, -1 otherwise
 */
int fillInfo(FileInfo* fi, FILE *file, char *file_name);

/**
 * @brief returns a chunk of the file's data
 * 
 * @param date file's data
 * @param start_index index where the chunk begins
 * @param quantity number of bytes of the chunk
 * 
 * @return data chunk
 */
unsigned char* dataChunk(unsigned char* data, int start_index, int quantity);

#endif