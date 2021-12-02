#ifndef BUFFER_UTILS_H
#define BUFFER_UTILS_H

#include "macrosLD.h"

int writeData(int fd, unsigned char *buf, int size);

void printData(unsigned char *buf, int size, int read);

void clean_buf(void *buf, int size);

#endif