#ifndef APP_RECEIVER_H
#define APP_RECEIVER_H

/**
 * @file receiver.h
 * @brief Application Layer - Receiver
 * @version 0.1
 *
 */
 #include "api.h"


/**
 * @brief Verifies if the packet received is a control packet
 * 
 * @param id START or END control packet
 * @param packet packet to be verified
 * 
 * @return 1 if the packet is a control packet, 0 otherwise
 */
 int checkControlPacket(enum packet_id id, unsigned char* packet);

/**
 * @brief Reads the Start Control packet from the serial port
 * 
 * @param fd file descriptor of connection port file
 * @param name name of the file to be sent
 * @param delay optional delay between reading the packet and sending the RR/REJ
 * @param genErrors the odd between 0-100000 that the function will manually change a received byte to generate a error
 * 
 * @return size of the file to be sent
 */
 unsigned long receiveStartPacket(int fd, unsigned char* name, unsigned int delay, unsigned int genErrors);

 #endif