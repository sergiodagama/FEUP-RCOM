#ifndef TRANSMITTER_H
#define TRANSMITTER_H

/**
 * @file transmitter.h
 * @brief Application Layer Transmitter
 * @version 0.1
 * 
 */
#include "api.h"

typedef struct ControlPacket {
    // unsigned char  C; //control value -> 2 for start, 3 for end
    // unsigned char T1;  //type of 0 -> file size, 1 -> file name
    // unsigned char L1;  //number of bytes of V field
    // unsigned char *V1;
    // unsigned char T2;  //type of 0 -> file size, 1 -> file name
    // unsigned char L2;  //number of bytes of V field
    // unsigned char *V2;

    unsigned char *packet;
    unsigned int size;

} ControlPacket;

int sendControlPacket(int fd, enum packet_id id, ControlPacket control_p);

int sendDataPacket(int fd, FileInfo* file_info);

ControlPacket createControlPacket(FileInfo* file_info);

#endif