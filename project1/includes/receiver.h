#ifndef RECEIVER_H
#define RECEIVER_H

/**
 * @file receiver.h
 * @brief Application Layer - Receiver
 * @version 0.1
 *
 */
 #include "api.h"

 int checkControlPacket(enum packet_id id, unsigned char* packet);

 unsigned long receiveStartPacket(int fd, unsigned char* name);

 #endif