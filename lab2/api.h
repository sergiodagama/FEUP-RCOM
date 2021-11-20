#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include "writer_utils.c"
#include "reader_utils.c"

enum flag {TRANSMITTER, RECEIVER};

enum state {CONNECTING, TRANSFERRING, DISCONNECTING, FINISHED};

int llopen(char * porta, enum flag flg);

int llread(int fd, char * buffer);

int llwrite(int fd, char * buffer, int length);

int llclose(int fd, enum flag flg);

#define ESCAPE 0x7d
#define FLAG 0x7e
#define VALUE_TO_XOR 0x20
#define FLAG_XORED (VALUE_TO_XOR ^ FLAG)
#define ESCAPE_XORED (VALUE_TO_XOR ^ ESCAPE)

char* byteStuffing(size_t size, unsigned char data[]);

char *reversByteStuffing(size_t size, unsigned char data[]);