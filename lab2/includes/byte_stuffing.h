#ifndef BYTE_STUFFING_H
#define BYTE_STUFFING_H

#include <stdlib.h>

/**
 * @file byte_stuffing.h
 * @brief Bytte Stuffing Module
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
 * @return stuffed data array with double of the size parameter
 */
unsigned char* byteStuffing(size_t size, unsigned char data[]);

/**
 * @brief Reverses byte stuffing on stuffed data array passed
 * 
 * @param size the size of the stuffed data array
 * @param stuffed the stuffed data array
 * @return array with half the size of the value passed
 */
unsigned char* reverseByteStuffing(size_t size, unsigned char stuffed[]);

#endif
