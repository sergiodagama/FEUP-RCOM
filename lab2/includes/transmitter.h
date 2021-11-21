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

int sendPacket(int fd, enum packet_id id, int Ns);

#endif