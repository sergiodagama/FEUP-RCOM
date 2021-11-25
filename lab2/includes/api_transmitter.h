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
#include "alarme.h"
#include "macrosLD.h"

struct termios oldtio,newtio;

// unsigned char SET[SU_TRAMA_SIZE] = {FLAG, A_EE, C_SET, BCC(A_EE, C_SET), FLAG};
// unsigned char UA_E[SU_TRAMA_SIZE] = {FLAG, A_EE, C_UA, BCC(A_EE, C_DISC), FLAG};
// unsigned char UA_R[SU_TRAMA_SIZE] = {FLAG, A_ER, C_UA, BCC(A_ER, C_UA), FLAG}; 
// unsigned char DISC_E[SU_TRAMA_SIZE] = {FLAG, A_EE, C_DISC, BCC(A_EE, C_DISC), FLAG};
// unsigned char DISC_R[SU_TRAMA_SIZE] = {FLAG, A_ER, C_DISC, BCC(A_ER, C_DISC), FLAG};

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