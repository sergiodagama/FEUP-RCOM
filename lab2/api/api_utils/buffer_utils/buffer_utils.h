#ifndef BUFFER_UTILS_H
#define BUFFER_UTILS_H

int writeData(int fd, unsigned char *buf, int size);

void printData(unsigned char *buf, int size);

void clean_buf(unsigned char *buf, int size);

#endif