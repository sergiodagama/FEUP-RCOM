/*Non-Canonical Input Processing*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>

#define BAUDRATE B38400
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

volatile int STOP=FALSE;

int receiverMsg(char st[], int fd){ 
    int res;
    int counter = 0;
    char *buffer;

    while (STOP==FALSE) {       /* loop for input */
        res = read(fd,buffer,1);    /* returns after 5 chars have been input */
        if(res==-1) return -1;
        st[counter] = *buffer;
        counter++;
        if (st[counter-1] =='\0') STOP=TRUE;
    }

    return counter;

}

int checkSET(char st[], int size){
    if (size != 5) return 1;

    int A = st[1];
    int C = st[2];
    int Bcc = st[3];
    
    if(A^C == Bcc) return 0;
    return 1;  
}



int main(int argc, char** argv)
{
    int fd,c, res;
    struct termios oldtio,newtio;
    char buf;

    if ( (argc < 2) || 
  	     ((strcmp("/dev/ttyS0", argv[1])!=0) && 
  	      (strcmp("/dev/ttyS1", argv[1])!=0) )) {
      printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
      exit(1);
    }


  /*
    Open serial port device for reading and writing and not as controlling tty
    because we don't want to get killed if linenoise sends CTRL-C.
  */
  
    
    fd = open(argv[1], O_RDWR | O_NOCTTY );
    if (fd <0) {perror(argv[1]); exit(-1); }

    if ( tcgetattr(fd,&oldtio) == -1) { /* save current port settings */
      perror("tcgetattr");
      exit(-1);
    }

    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    /* set input mode (non-canonical, no echo,...) */
    newtio.c_lflag = 0;

    newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
    newtio.c_cc[VMIN]     = 1;   /* blocking read until 5 chars received */



  /* 
    VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a 
    leitura do(s) próximo(s) caracter(es)
  */



    tcflush(fd, TCIOFLUSH);

    if ( tcsetattr(fd,TCSANOW,&newtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }

    printf("New termios structure set\n");
    
    char st[5];
    int counter = 0;
    char *buffer;

    while (STOP==FALSE) {       /* loop for input */
      res = read(fd,buffer,1);    /* returns after 5 chars have been input */
      st[counter] = *buffer;
      counter++;
      if (st[counter-1] =='\0') STOP=TRUE;
    }

    if(checkSET(st, counter) == 1){
        printf("SET invalid \n");
        return 1;
    }
    
    
    printf("NICE\n");
    

    
    

    
    int bytes = write(fd, st, counter);
    printf("Bytes Written: %d\n", bytes);

    tcsetattr(fd,TCSANOW,&oldtio);
    close(fd);
    return 0;
}
