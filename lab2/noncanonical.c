/*Non-Canonical Input Processing*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>

#include "macrosLD.h"

#define BAUDRATE B38400
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

volatile int STOP=FALSE;

unsigned char UA[TRAMA_SIZE] = {FLAG, A_ER, C_UA, BCC(A_ER, C_UA), FLAG}; 


int checkSETByteRecieved(unsigned char byte_recieved, int idx){
  int is_OK = FALSE;

  
  if((idx == 0 || idx == 4) && byte_recieved == FLAG){
    is_OK = TRUE;
  }
  else if (idx == 1 && byte_recieved == A_EE){
    is_OK = TRUE;
  }
  else if (idx == 2 && byte_recieved == C_SET){
    is_OK = TRUE;
  }
  else if (idx == 3 && byte_recieved == BCC(A_EE, C_SET)){
    is_OK = TRUE;
  }

  return is_OK;
}

int writeUA(int fd){

    int res, i = 0;
    while (i < TRAMA_SIZE){
      printf("Written - 0x%x\n", UA[i]);
      res = write(fd, &UA[i], 1);
      i++;
    }

    return res;
}

int main(int argc, char** argv)
{
    int fd,c, res;
    struct termios oldtio,newtio;

    if ( (argc < 2) || 
  	     ((strcmp("/dev/ttyS0", argv[1])!=0) && 
  	      (strcmp("/dev/ttyS1", argv[1])!=0) && 
  	      (strcmp("/dev/ttyS11", argv[1])!=0) )) {
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
    leitura do(s) pr�ximo(s) caracter(es)
  */



    tcflush(fd, TCIOFLUSH);

    if ( tcsetattr(fd,TCSANOW,&newtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }

  
    unsigned char rSET[TRAMA_SIZE];
    int idx = 0;
    int CONNECTED=FALSE;


    //while - open connection
    while(!CONNECTED){

      //Receção do SET
      while (!STOP) {       /* loop for input */
        res = read(fd,&rSET[idx],1);  

        printf("0x%x : %d\n", rSET[idx], res);

        //Check se os valores são iguais aos expected -> se sim continua normalmente se não vai mudar o idx para repetir leitura

        if (idx == 5) STOP = TRUE;

        idx++;
      }
      STOP = FALSE;

      if(rSET[2] == C_NS0) { //é trama de INFO
          CONNECTED = TRUE;
      }

      if(checkSETRecieved(rSET[idx], idx) == TRUE) 

    printf("All OK on receiver!\n");

    sleep(2);
    printf("\n");

    //Envio de UA
    if(writeUA(fd) < 0)
      perror("Error writing UA\n");

