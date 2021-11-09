#define I
#define 
#define RR
#define RR

enum flag {TRANSMITTER, RECEIVER};

int llopen(int porta, flag);

int llread(int fd, char * buffer);

int llwrite(int fd, char * buffer, int length);

int llclose(int fd);