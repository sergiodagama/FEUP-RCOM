#include "api_receiver.h"

volatile int STOP = FALSE;

unsigned char buf_R[MAX_SIZE];

extern enum state state_receiver;

int llopen_receiver(char * port, int* fid){
  int fd, c, res;

  /*
    Open serial port device for reading and writing and not as controlling tty
    because we don't want to get killed if linenoise sends CTRL-C.
  */

  fd = open(port, O_RDWR | O_NOCTTY);

  *fid = fd;  //to minimize changes in the code that already has fd

  if(fd < 0){
    perror(port); 
    return ERROR; 
  }

  if(tcgetattr(fd, &oldtio) == -1){ /* save current port settings */
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

  if(tcsetattr(fd, TCSANOW, &newtio) == -1) {
    perror("tcsetattr");
    return ERROR;
  }

  printf("New termios structure set\n");

  //openned port, starting connection procedure now

  int idx = 0;
  state_receiver = CONNECTING;

  while (state_receiver == CONNECTING){
    //Receção do SET
    printf(" - Receiving SET...\n");
    while (!STOP)
    { /* loop for input */

      if((idx == 0) && (SET_received == 1)){
        clean_buf(buf_R,MAX_SIZE);
      }
      res = read(fd, &buf_R[idx], 1);

      //Check se os valores são iguais aos expected -> se sim continua normalmente se não vai mudar o idx para repetir leitura
      if( (idx == 0) && (SET_received == 1) && (buf_R[idx] == 0x02)){ //verifica se já está a receber o control packet START
        idx++;
        state_receiver=TRANSFERRING;
        //receive_control_packet(buf_R, fd, ...);
        break;
      }

      if (checkSETByteRecieved(buf_R[idx], idx) == TRUE) //verifica se está a receber os bytes do SET corretos
        idx++;
      else idx = 0; //volta ao início?

      if (idx == 5){ 
        STOP = TRUE;
        state_receiver=TRANSFERRING; //com isto domentado o programa corre, porém na versão final tem q esta descomentado
      }
    }

    if (STOP == TRUE) //se recebeu o SET corretamente, envia o UA para o Transmitter
    {
      //só faz print se valor correto
      printTramaRead(buf_R, SU_TRAMA_SIZE);

      sleep(2);
      printf("\n");

      //Envio de UA
      printf(" - Sending UA\n");
      if (writeData(fd, UA_R, SU_TRAMA_SIZE) < 0)
        perror("    Error writing UA\n");

      printf("\nAll OK on receiver!\n");
    }

    STOP = FALSE;
  }

  return ALL_OK;
}

int llclose_receiver(int fd){
  if(tcsetattr(fd, TCSANOW, &oldtio) == -1) {
    perror("tcsetattr");
    return ERROR;
  }

  close(fd);
  
  return ALL_OK;
}