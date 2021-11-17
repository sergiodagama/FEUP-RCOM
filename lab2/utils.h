#include <unistd.h>
#include <signal.h>
#include <stdio.h>

int writeData(int fd, unsigned char *trama, int size);

int readData(int fd, int *flag, unsigned char *reader, int size);

int checkTramaReceived(unsigned char *trama, int size, int typeMsg);