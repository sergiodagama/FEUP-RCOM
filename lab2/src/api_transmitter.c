#include "../includes/api_transmitter.h"
// #include "../includes/macrosLD.h"


volatile int STOP=FALSE;

extern int flag, connect_attempt; 

unsigned char buf_E[MAX_SIZE];

enum state state_transmitter;

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

int llopen_transmitter(char* port, int *fid){
  int fd, c, res;
  struct termios oldtio, newtio;

  /*
    Open serial port device for reading and writing and not as controlling tty
    because we don't want to get killed if linenoise sends CTRL-C.
  */

  fd = open(port, O_RDWR | O_NOCTTY);

  *fid = fd;

  if(fd < 0){
    perror(port);
    return ERROR; 
  }

  if (tcgetattr(fd, &oldtio) == -1) { /* save current port settings */
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

  //port file open starting connection procedure

  signal(SIGALRM, atende);  // instala a rotina que atende interrupcao
  siginterrupt(SIGALRM, 1); // quando o sinal SIGALRM é apanhado, provoca uma interrupção no read()
  
  int idx;

  connect_attempt = 1;

  state_transmitter = CONNECTING;

  //while - open connection
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
  
    //Rececao do UA
    idx = 0;
    alarm(ALARM_SECONDS);
    flag = 0;

    printf(" - Receiving UA...\n");
    while (!STOP) {       /* loop for input */

      //printf("before read\n");
      if ((res = read(fd, &buf_E[idx], 1)) < 0){
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
      else  idx = 0; //volta ao início?
      
      if (idx == 5){
        STOP = TRUE;
        state_transmitter = TRANSFERRING;
      }
    }

    alarm(0); //Reset alarm

    if (STOP == TRUE){
        //só faz print se valor correto
        printTramaRead(buf_E, SU_TRAMA_SIZE);
    }
  }

  printf("\nAll OK on sender!\n");

  return ALL_OK;
}

int llclose_transmitter(int fd){


    STOP = FALSE;

    connect_attempt = 1;

    int res, idx=0;

    //starting to disconnect

    state_transmitter=DISCONNECTING; //temporário!! serve para o codigo entrar no while

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

      //receive DISC
      printf(" - Receiving DISC_R...\n");
        
      idx = 0;
      alarm(ALARM_SECONDS);
      flag = 0;
      while(!STOP){
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

        if (idx == 5){
          STOP = TRUE;
          state_transmitter = FINISHED;
        }
      }
      alarm(0);//TODO timeout

    }
  
    printTramaRead(buf_E, SU_TRAMA_SIZE);//só faz print se valor correto

      //send UA
    printf(" - Sending UA_E...\n");

    if (writeData(fd, UA_E, SU_TRAMA_SIZE) < 0)
      perror("    Erro sending disconnect UA\n");

    printf("    UA sent, Disconnecting... bye bye\n");

    if(tcsetattr(fd, TCSANOW, &oldtio) == -1){
      perror("tcsetattr");
      return -1;
    }

    close(fd);
    return ALL_OK;
}
