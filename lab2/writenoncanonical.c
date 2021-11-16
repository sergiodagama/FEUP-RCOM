/*Non-Canonical Input Processing*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>

#include "macrosLD.h"
#include "alarme.c"

#define BAUDRATE B38400
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

volatile int STOP=FALSE;

extern int flag, connect_attempt; 

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


    signal(SIGALRM, atende);  // instala a rotina que atende interrupcao
    siginterrupt(SIGALRM, 1); // quando o sinal SIGALRM é apanhado, provoca uma interrupção no read()
    
    unsigned char rUA[TRAMA_SIZE];
    int idx;

    connect_attempt = 1;

    while(!STOP){
      //Enviar o SET

      if (connect_attempt > 3){
        printf("Sender gave up, attempts exceded\n");
        return 1;
      }

      if(writeSET(fd) < 0)
        perror("Error writing SET\n");


      //printf("here after write\n");

    
      //Rececao do UA
      idx = 0;
      alarm(3);
      flag = 0;

      while (!STOP) {       /* loop for input */

        //printf("before read\n");
        if ((res = read(fd,&rUA[idx],1)) < 0){
          if (flag == 1){
            printf("Timed Out\n");
            break;
          }
          else{
            perror("Read failed\n");
          }
        }

        printf("0x%x : %d\n", rUA[idx], res);

        //Check se os valores são iguais aos expected -> se sim continua normalmente se não vai mudar o idx para repetir leitura

        if(checkUAByteRecieved(rUA[idx], idx) == TRUE) //Depois a state machine vai ligar aqui
          idx++;
        else 
          idx = 0; //volta ao início?
        
        if (idx == 5) STOP = TRUE;
      }

      alarm(0); //Reset alarm

    }

    printf("All OK on sender!\n");

   
       sleep(1);
       
    if ( tcsetattr(fd,TCSANOW,&oldtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }




    close(fd);
    return 0;
}
