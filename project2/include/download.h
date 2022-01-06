#ifndef _DOWNLOAD_H_
#define _DOWNLOAD_H_

#include <stdio.h> 
#include <stdlib.h> 
#include <errno.h> 
#include <netdb.h> 
#include <sys/types.h>
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <libgen.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>


#include "input.h"


#define SERVER_PORT 6000
#define SERVER_ADDR "192.168.28.96"

#define SERVER_PORT 21
#define BUFFER_SIZE 1024
#define MAX_STR_LEN 1024

#endif // _DOWNLOAD_H_