#ifndef API_TRANSMITTER_H
#define API_TRANSMITTER_H


/**
 * @file api_transmitter.h
 * @brief API llopen and llclose for transmitter
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
#include <signal.h>
#include <errno.h>
#include "macrosLD.h"

struct termios oldtio,newtio;

/**
 * @brief SIGALRM handler
 */
void atende();

/**
 * @brief Verifies if the buf's bytes are equivalent to a RR 
 * 
 * @param buf buf to be verified
 * @param index index of the byte to be verified
 * @param Ns sequence number
 * 
 * @return 0 in case of error, 1 otherwise
 */
int checkRRByteRecieved(unsigned char* buf, int index, int Ns);

/**
 * @brief Verifies if the buf's bytes are equivalent to a REJ 
 * 
 * @param c byte to be verified
 * @param index index of the byte to be verified
 * @param Ns sequence number
 * 
 * @return 0 in case of error, 1 otherwise
 */
int isRej(unsigned char c, int index,  int Ns);

/**
 * @brief Verifies if the buf's bytes are equivalent to a UA sent by the receiver
 * 
 * @param byte_recieved byte to be verified
 * @param idx index of the byte to be verified
 * 
 * @return 0 in case of error, 1 otherwise
 */
int checkUAByteRecieved(unsigned char byte_recieved, int idx);

/**
 * @brief Verifies if the buf's bytes are equivalent to a Disc sent by the receiver
 * 
 * @param byte_recieved byte to be verified
 * @param idx index of the byte to be verified
 * 
 * @return 0 in case of error, 1 otherwise
 */
int checkDiscRByteRecieved(unsigned char byte_recieved, int idx);

/**
 * @brief Opens connection on the transmitter side
 * 
 * @param port the name of the port file
 * @param fid the file descriptor to be passed to higher level functions (llopen)
 * @return negative value in case of error, positive otherwise
 */
int llopen_transmitter(char* port, int* fid);

/**
 * @brief Closes connection on the transmitter side
 * 
 * @param fd file descriptor of connection port file
 * @return negative value in case of error, positive otherwise
 */
int llclose_transmitter(int fd);

#endif