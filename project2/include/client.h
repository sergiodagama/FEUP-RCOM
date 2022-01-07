#ifndef CLIENT_H_ 
#define CLIENT_H_ 

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

#define SERVER_PORT 21

void write_cmd(int sockfd, char *cmd, char *arg);

char * read_reply(int sockfd);

int give_credentials(url_data *data, int sockfd);

int connect_socket(char * ip_addr, int port);

int activate_passive_mode(int sockfd);

int download_file(int sockfd, char * url_path);

#endif