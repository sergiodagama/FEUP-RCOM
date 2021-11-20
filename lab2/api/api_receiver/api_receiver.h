#ifndef API_RECEIVER_H
#define API_RECEIVER_H

/**
 * @file api_receiver.h
 * @brief API llopen and llclose for receiver
 * @version 0.1
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include "../macrosLD.h"


#define BAUDRATE B38400

struct termios oldtio, newtio;

/**
 * @brief Opens connection on the receiver side
 * 
 * @param port the name of the port file
 * @param fid the file descriptor to be passed to higher level functions (llopen)
 * @return negative value in case of error, positive otherwise
 */
int llopen_receiver(char* port, int* fid);

/**
 * @brief Closes connection on the receiver side
 * 
 * @param fd file descriptor of connection port file
 * @return negative value in case of error, positive otherwise
 */
int llclose_receiver(int fd);

#endif