#ifndef BYTE_STUFFING_H
#define BYTE_STUFFING_H

#include <stdlib.h>

/**
 * @file byte_stuffing.h
 * @brief Byte Stuffing Module
 * @version 0.1
 * 
 */
#define ESCAPE 0x7d
#define FLAG 0x7e
#define VALUE_TO_XOR 0x20
#define FLAG_XORED (VALUE_TO_XOR ^ FLAG)
#define ESCAPE_XORED (VALUE_TO_XOR ^ ESCAPE)

/**
 * @brief Applies byte stuffing to the data array
 * 
 * @param size the size of the data array
 * @param data the data array to be stuffed
 * @param stuffed where stuffed frame will be stored
 * 
 * @return 1 if successful, -1 otherwise
 */
int byteStuffing(size_t size, unsigned char data[], unsigned char* stuffed);

/**
 * @brief Reverses byte stuffing on stuffed data array passed
 * 
 * @param size the size of the stuffed data array
 * @param stuffed the stuffed data array
 * @param original where the destuffed frame will be stored
 * 
 * @return 1 if successful, -1 otherwise
 */
int reverseByteStuffing(int *size, unsigned char stuffed[], unsigned char* original);

#endif
