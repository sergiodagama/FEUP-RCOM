#include "../includes/api_transmitter.h"

int connect_attempt =1;

volatile int STOP=FALSE;

unsigned char buf_E[MAX_SIZE];

enum state state_transmitter;

void atende()                  
{
   connect_attempt++;
}

int checkRRByteRecieved(unsigned char* buf, int index, int Ns){

    if ((index == 0 && buf[0] == FLAG) || (index == 4 && buf[4] == FLAG)){
        return TRUE;        
    }
    if(index == 1 && buf[1] == A_EE){
        return TRUE;
    }
    if(index == 2 && buf[2] == C_RR_NS1){
        return TRUE;
    }
    if(index == 2 && buf[2] == C_RR_NS0){
        return TRUE;
    }
    if(index == 3 && (buf[3] == (A_EE^C_RR_NS1))){
        return TRUE;
    }
    if(index == 3 && (buf[3] == (A_EE^C_RR_NS0))){
        return TRUE;
    }

    return FALSE;

}


int isRej(unsigned char c, int index,  int Ns){
   
    if(index == 2){
        if(c == 0x81 || c == 0x01) return TRUE;    
    }
    else if (index == 3) {
      if (c == 0x82 || c == 0x02) return TRUE;
        
    }
    return FALSE;
}

int checkUAByteRecieved(unsigned char byte_recieved, int idx){
  int is_OK = FALSE;

  
  if((idx == 0 || idx == 4) && byte_recieved == FLAG){
    is_OK = TRUE;
  }
  else if (idx == 1 && byte_recieved == A_EE){
    is_OK = TRUE;
  }
  else if (idx == 2 && byte_recieved == C_UA){
    is_OK = TRUE;
  }
  else if (idx == 3 && byte_recieved == BCC(A_EE, C_UA)){
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

int llopen_transmitter(char* port, int *fid){
  int fd, c, res;


  fd = open(port, O_RDWR | O_NOCTTY);

  *fid = fd;

  if(fd < 0){
    perror(port);
    return ERROR; 
  }

  if (tcgetattr(fd, &oldtio) == -1) {
    perror("tcgetattr");
    return ERROR;
  }

  bzero(&newtio, sizeof(newtio));
  newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
  newtio.c_iflag = IGNPAR;
  newtio.c_oflag = 0;

  /* set input mode (non-canonical, no echo,...) */
  newtio.c_lflag = 0;
  newtio.c_cc[VTIME] = 0;   /* inter-character timer unused */
  newtio.c_cc[VMIN] = 1;   /* blocking read until 1 chars received */

  /* 
    VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a 
    leitura do(s) pr�ximo(s) caracter(es)
  */

  tcflush(fd, TCIOFLUSH);

  if(tcsetattr(fd, TCSANOW, &newtio) == -1){
    perror("tcsetattr");
    return ERROR;
  }

  printf("New termios structure set\n");

  signal(SIGALRM, atende);
  siginterrupt(SIGALRM, 1); 
  
  int idx;

  connect_attempt = 1;

  state_transmitter = CONNECTING;

  while(state_transmitter == CONNECTING){

    if (connect_attempt > MAX_ATTEMPS){
      printf("Sender gave up, attempts exceded\n");
      //llclose(fd, TRANSMITTER);
      return ERROR;
    }

    printf("Attempt %d\n - Sending SET\n", connect_attempt);
    if(writeData(fd, SET, SU_TRAMA_SIZE) < 0){
      perror("    Error writing SET\n");
    }

    printf("\n");
  
    
    idx = 0;
    alarm(ALARM_SECONDS);

    printf(" - Receiving UA...\n");
    while (!STOP) {  

      if ((res = read(fd, &buf_E[idx], 1)) < 0){
        if (errno == EINTR){
          printf("    Timed Out\n\n");
          break;
        }
        else{
          perror("    Read failed\n\n");
        }
      }

      if(checkUAByteRecieved(buf_E[idx], idx) == TRUE) 
        idx++;
      else  idx = 0; 
      
      if (idx == 5){
        STOP = TRUE;
        state_transmitter = TRANSFERRING;
      }
    }

    alarm(0); 

    // if (STOP == TRUE){
    //     printData(buf_E, SU_TRAMA_SIZE, READ);
    // }
  }

  printf("\nAll OK on sender!\n");

  return ALL_OK;
}

int llclose_transmitter(int fd){


    STOP = FALSE;

    connect_attempt = 1;

    int res, idx=0;


    state_transmitter=DISCONNECTING; 

    while(state_transmitter==DISCONNECTING){

      if(connect_attempt > 4){
        printf("    Attempt %d", connect_attempt);
        return ERROR;
      }
      //send DISC
      printf(" - Sending DISC_E...\n");
      
      if(writeData(fd, DISC_E, SU_TRAMA_SIZE) < 0){
        perror("    Error writing DISC\n");
      }

      printf(" - Receiving DISC_R...\n");
        
      idx = 0;
      alarm(ALARM_SECONDS);
      
      while(!STOP){
        if ((res = read(fd, &buf_E[idx], 1)) < 0){
          if (errno == EINTR){
            printf("    Timed Out\n\n");
            break;
          }
          else{
            perror("    Read failed\n\n");
          }
        }

        if(checkDiscRByteRecieved(buf_E[idx], idx) == TRUE) 
          idx++;
        else 
          idx = 0; //volta ao início?

        if (idx == 5){
          STOP = TRUE;
          state_transmitter = FINISHED;
        }
      }
      alarm(0);

    }
  
    //printData(buf_E, SU_TRAMA_SIZE, READ);

     
    printf(" - Sending UA_E...\n");

    if (writeData(fd, UA_E, SU_TRAMA_SIZE) < 0)
      perror("    Erro sending disconnect UA\n");

    printf("    UA sent, Disconnecting... bye bye\n");

    sleep(2);

    if(tcsetattr(fd, TCSANOW, &oldtio) == -1){
      perror("tcsetattr");
      return -1;
    }

    close(fd);
    return ALL_OK;
}
