#ifndef API_H
#define API_H


/**
 * @file api.h
 * @brief Application Protocol Interface 
 * @version 0.1
 * 
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>

#include "api_receiver.h"
#include "api_transmitter.h"
#include "byte_stuffing.h"
#include "file.h"

/**
 * @brief Application type
 * 
 */
enum status {TRANSMITTER, RECEIVER};

//controls the connection state in both sides separately
extern enum state state_receiver;
extern enum state state_transmitter;


/**
 * @brief Used by the application layer to describe its use of the API
 * 
 */
typedef struct ApplicationLayer {
    int fileDescriptor; //file descriptor correspondant to the serial port
    enum status status; // TRANSMITTER or RECEIVER
} ApplicationLayer;


/**
 * @brief Opens a connection
 * 
 * @param port the port file name where the connection is taking place
 * @param status the type of application (transmitter or receiver)
 * @param fid the file descriptor to be passed to higher level functions (llopen)
 * @return positive value when no errors, negative value in case of error
 */
int llopen(char* port, enum status stat, int* fid);

/**
 * @brief Reads a data frame and sends RR or REJ
 * 
 * @param fd file descriptor of connection port file
 * @param buffer characters array to be received
 * @return number of characters read, negative value in case of error
 */
int llread(int fd, unsigned char* buffer);

/**
 * @brief Writes a data frame and receives response RR or REJ
 * 
 * @param fd file descriptor of connection port file
 * @param buffer characters array to be received
 * @param length characters array size
 * @param Ns current Ns
 * @return written characters, negative value in case of error
 */
int llwrite(int fd, unsigned char* buffer, int length);

/**
 * @brief Closes a connection
 * 
 * @param fd file descriptor of connection port file
 * @param status the type of application (transmitter or receiver)
 * @return positive value when there are no errors, negative value otherwise
 */
int llclose(int fd, enum status stat);

#endif