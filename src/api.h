#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

enum flag {TRANSMITTER, RECEIVER};

int llopen(int porta, enum flag flg);

int llread(int fd, char * buffer);

int llwrite(int fd, char * buffer, int length);

int llclose(int fd);

#define ESCAPE 0x7d
#define FLAG 0x7e
#define VALUE_TO_XOR 0x20
#define FLAG_XORED (VALUE_TO_XOR ^ FLAG)
#define ESCAPE_XORED (VALUE_TO_XOR ^ ESCAPE)

char* byteStuffing(size_t size, unsigned char data[], size_t* trimmed_size);