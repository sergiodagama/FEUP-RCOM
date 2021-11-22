#ifndef TRANSMITTER_H
#define TRANSMITTER_H

/**
 * @file transmitter.h
 * @brief Application Layer Transmitter
 * @version 0.1
 * 
 */
#include "api.h"

enum packet_id {START, DATA, END};

int sendPacket(int fd, enum packet_id id, FileInfo* file_info);

typedef struct ControlPacket {
    unsigned char  C; //control value -> 2 for start, 3 for end
    unsigned char T1;  //type of 0 -> file size, 1 -> file name
    unsigned char L1;  //number of bytes of V field
    unsigned char *V1;
    unsigned char T2;  //type of 0 -> file size, 1 -> file name
    unsigned char L2;  //number of bytes of V field
    unsigned char *V2;
} ControlPacket;

#endif