#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>

#define BAUDRATE B38400

struct termios oldtio,newtio;

int llopen_writer(char * porta);

int llclose_writer(int fd);
