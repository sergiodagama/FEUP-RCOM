/*Non-Canonical Input Processing*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>

#include "macrosLD.h"
#include "alarme.c"
#include "utils.c"

#define BAUDRATE B38400
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

volatile int STOP=FALSE;

extern int flag, connect_attempt; 

unsigned char buf_E[MAX_SIZE];

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

int checkDiscRByteRecieved(unsigned char byte_recieved, int idx){
  int is_OK = FALSE;

  
  if((idx == 0 || idx == 4) && byte_recieved == FLAG){
    is_OK = TRUE;
  }
  else if (idx == 1 && byte_recieved == A_ER){
    is_OK = TRUE;
  }
  else if (idx == 2 && byte_recieved == C_DISC){
    is_OK = TRUE;
  }
  else if (idx == 3 && byte_recieved == BCC(A_ER, C_DISC)){
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
    
    int idx;

    connect_attempt = 1;

    //while - open connection
    while(!STOP){

      if (connect_attempt > MAX_ATTEMPS){
        printf("Sender gave up, attempts exceded\n");
        return 1;
      }

      printf("Attempt %d\n - Sending SET\n", connect_attempt);
      if(writeData(fd, SET, SU_TRAMA_SIZE) < 0)
        perror("    Error writing SET\n");

      printf("\n");
    
      //Rececao do UA
      idx = 0;
      alarm(ALARM_SECONDS);
      flag = 0;

      printf(" - Receiving UA...\n");
      while (!STOP) {       /* loop for input */

        //printf("before read\n");
        if ((res = read(fd,&buf_E[idx],1)) < 0){
          if (flag == 1){
            printf("    Timed Out\n\n");
            break;
          }
          else{
            perror("    Read failed\n\n");
          }
        }

        //Check se os valores são iguais aos expected -> se sim continua normalmente se não vai mudar o idx para repetir leitura

        if(checkUAByteRecieved(buf_E[idx], idx) == TRUE) //Depois a state machine vai ligar aqui
          idx++;
        else 
          idx = 0; //volta ao início?
        
        if (idx == 5) STOP = TRUE;
      }

      alarm(0); //Reset alarm

      if (STOP == TRUE){
         //só faz print se valor correto
         printTramaRead(buf_E, SU_TRAMA_SIZE);
      }
    }

    printf("\nAll OK on sender!\n");

   
    sleep(1);
       

    clean_buf(buf_E, MAX_SIZE);

    connect_attempt = 1;
    STOP = FALSE;

    //while - data transmission
    // while(!STOP){

    //   if (connect_attempt > MAX_ATTEMPS){
    //     printf("Sender gave up, attempts exceded\n");
    //     return 1;
    //   }

    //   if(writeData(fd, SET, SU_TRAMA_SIZE) < 0)
    //     perror("Error writing SET\n");

    
    //   //Rececao do UA
    //   idx = 0;
    //   alarm(ALARM_SECONDS);
    //   flag = 0;

    //   while (!STOP) {       /* loop for input */

    //     //printf("before read\n");
    //     if ((res = read(fd,&rUA[idx],1)) < 0){
    //       if (flag == 1){
    //         printf("Timed Out\n");
    //         break;
    //       }
    //       else{
    //         perror("Read failed\n");
    //       }
    //     }

    //     printf("0x%x : %d\n", rUA[idx], res);

    //     //Check se os valores são iguais aos expected -> se sim continua normalmente se não vai mudar o idx para repetir leitura

    //     if(checkUAByteRecieved(rUA[idx], idx) == TRUE) //Depois a state machine vai ligar aqui
    //       idx++;
    //     else 
    //       idx = 0; //volta ao início?
        
    //     if (idx == 5) STOP = TRUE;
    //   }

    //   alarm(0); //Reset alarm

    // }

    //!!!!!!!!!!!!!!!!!!!!NEWWWWWW!!!!!!!!!!!!!!!!!!!!!!!!
    //while - disconnect
    int DISCONNECT = FALSE, RECEIVED = FALSE, SENT = FALSE;
    STOP = FALSE;

    connect_attempt = 1;

    //starting to disconnect
    while(!DISCONNECT){

      if(connect_attempt > 4){
        printf("    Attempt %d", connect_attempt);
        return 1;
      }
      //send DISC
      printf(" - Sending DISC_E...\n");
      
      if(writeData(fd, DISC_E, SU_TRAMA_SIZE) < 0){
        perror("    Error writing DISC\n");
      }

        

      //receive DISC
      printf(" - Receiving DISC_R...\n");

        
      idx = 0;
      alarm(ALARM_SECONDS);
      flag = 0;
      while(!DISCONNECT){
        if((res = read(fd,&buf_E[idx],1))<0){
          if (flag == 1){
            printf("    Timed Out\n\n");
            break;
          }
          else{
            perror("    Read failed\n\n");
          }
        }

        //Check se os valores são iguais aos expected -> se sim continua normalmente se não vai mudar o idx para repetir leitura
        if(checkDiscRByteRecieved(buf_E[idx], idx) == TRUE) 
          idx++;
        else 
          idx = 0; //volta ao início?

        if (idx == 5) DISCONNECT = TRUE;
      }
      alarm(0);//TODO timeout

    }
  
    if (DISCONNECT == TRUE)
         printTramaRead(buf_E, SU_TRAMA_SIZE);//só faz print se valor correto

      //send UA
    printf(" - Sending UA_E...\n");

    if (writeData(fd, UA_E, SU_TRAMA_SIZE) < 0)
      perror("    Erro sending disconnect UA\n");

    printf("    UA sent, Disconnecting... bye bye\n");
    //!!!!!!!!!!!!!!!!!!!!NEWWWWWW!!!!!!!!!!!!!!!!!!!!!!!!

    if ( tcsetattr(fd,TCSANOW,&oldtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }




    close(fd);
    return 0;
}
