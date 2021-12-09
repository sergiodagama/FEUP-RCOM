#ifndef APP_TRANSMITTER_H
#define APP_TRANSMITTER_H

/**
 * @file transmitter.h
 * @brief Application Layer Transmitter
 * @version 0.1
 * 
 */
#include "api.h"


/**
 * @brief Control Packet information
 */
typedef struct ControlPacket {

    unsigned char *packet;      // array to be sent
    unsigned int size;          // size of the array

} ControlPacket;

/**
 * @brief Sends a control packet to the serial port
 * 
 * @param fd file descriptor of connection port file
 * @param id identifies the control packet, either START or END
 * @param control_p control packet to be sent
 * 
 * @return bytes sent
 */
int sendControlPacket(int fd, enum packet_id id, ControlPacket control_p);

/**
 * @brief Sends data packets to the serial port
 * 
 * @param fd file descriptor of connection port file
 * @param file_info file's informations and data
 * 
 * @return 0 if successful, otherwise returns the number of bytes of the file that weren't sent
 */
int sendDataPacket(int fd, FileInfo* file_info);

/**
 * @brief Constructs the control packet 
 * 
 * @param file_info file's informations and data
 * 
 * @return control packet
 */
ControlPacket createControlPacket(FileInfo* file_info);

#endif