#ifndef BUFFER_UTILS_H
#define BUFFER_UTILS_H

#include "macrosLD.h"


/**
 * @brief writes data on the serial port 
 * 
 * @param fi serial port file descriptor
 * @param buf array to be sent
 * @param size size of the array
 * 
 * @return number of bytes sent if successful, otherwise -1
 */
int writeData(int fd, unsigned char *buf, int size);

/**
 * @brief prints a data frame on the console
 * 
 * @param trama trama to be printed
 * @param size  size of the trama
 * @param read  1 to print "r" before each byte, otherwise 0 to print "wr"
 */
void printData(unsigned char *trama, int size, int read);

/**
 * @brief puts every byte on a buffer equal to 0x00
 * 
 * @param buf buffer to be cleaned
 * @param size size of the buffer
 */
void clean_buf(void *buf, int size);

#endif