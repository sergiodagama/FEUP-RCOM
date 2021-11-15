/*Non-Canonical Input Processing*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>

#include "macrosLD.h"

#define BAUDRATE B38400
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

volatile int STOP=FALSE;

unsigned char SET[TRAMA_SIZE] = {FLAG, A_EE, C_SET, BCC(A_EE, C_SET), FLAG};

int writeSET(int fd){

    int res, i = 0;
    while (i < TRAMA_SIZE){
      printf("Written - 0x%x\n", SET[i]);
      res = write(fd, &SET[i], 1);
      i++;
    }

    return res;
}


int checkUAByteRecieved(unsigned char byte_recieved, int idx){
  int is_OK = FALSE;

  
  if((idx == 0 || idx == 4) && byte_recieved == FLAG){
    is_OK = TRUE;
  }
  else if (idx == 1 && byte_recieved == A_ER){
    is_OK = TRUE;
  }
  else if (idx == 2 && byte_recieved == C_UA){
    is_OK = TRUE;
  }
  else if (idx == 3 && byte_recieved == BCC(A_ER, C_UA)){
    is_OK = TRUE;
  }

  return is_OK;
}

int writeI(int fd, unsigned char *I, int size){
  int res, i = 0;
    while (i < size){
      printf("Written - 0x%x\n", I[i]);
      res = write(fd, &I[i], 1);
      i++;
    }

    return res;
}

int main(int argc, char** argv)
{
    int fd,c, res;
    struct termios oldtio,newtio;
    int sum = 0, speed = 0;
    
    if ( (argc < 2) || 
  	     ((strcmp("/dev/ttyS0", argv[1])!=0) && 
  	      (strcmp("/dev/ttyS1", argv[1])!=0) && 
  	      (strcmp("/dev/ttyS10", argv[1])!=0) )) {
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
    newtio.c_cc[VMIN]     = 1;   /* blocking read until 1 chars received */



  /* 
    VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a 
    leitura do(s) pr�ximo(s) caracter(es)
  */



    tcflush(fd, TCIOFLUSH);

    if ( tcsetattr(fd,TCSANOW,&newtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }

    printf("New termios structure set\n");
    

    //Enviar o SET
    if(writeSET(fd) < 0)
      perror("Error writing SET\n");
   
   //Rececao do UA
   
    unsigned char rUA[TRAMA_SIZE];
    int idx = 0;

    while (!STOP) {       /* loop for input */
      res = read(fd,&rUA[idx],1);  

      printf("0x%x : %d\n", rUA[idx], res);

      //Check se os valores são iguais aos expected -> se sim continua normalmente se não vai mudar o idx para repetir leitura

      if(checkUAByteRecieved(rUA[idx], idx) == TRUE) //Depois a state machine vai ligar aqui
        idx++;
      else 
        idx = 0; //volta ao início?
      
      if (idx == 5) STOP = TRUE;
    }

    printf("All OK on sender!\n");

   
    sleep(1);

    const unsigned int data_size = 5; //para exemplo -> meter esta como global(?)
    unsigned char D[5] = {0x10, 0x11, 0x12, 0x13, 0x14};
    unsigned char BCC2 = BCC(BCC(BCC(D[1], D[2]), D[3]), D[4]); 
    unsigned char Info[11] = {FLAG, A_EE, C_NS0, BCC(A_EE, C_NS0), D[0], D[1], D[2], D[3], D[4], BCC2, FLAG}; //resolver o size

 //colocar depois em ciclo
    //Enviar I
    if(writeI(fd, Info, TRAMA_I_SIZE(data_size)) < 0)
      perror("Error writing I\n");


    if ( tcsetattr(fd,TCSANOW,&oldtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }

    close(fd);
    return 0;
}
