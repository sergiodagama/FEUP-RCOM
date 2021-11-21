#include "../includes/api_receiver.h"

static int STOP = FALSE;

unsigned char buf_R[MAX_SIZE];

enum state state_receiver;

int checkSETByteRecieved(unsigned char byte_recieved, int idx)
{
  int is_OK = FALSE;

  if ((idx == 0 || idx == 4) && byte_recieved == FLAG)
  {
    is_OK = TRUE;
  }
  else if (idx == 1 && byte_recieved == A_EE)
  {
    is_OK = TRUE;
  }
  else if (idx == 2 && byte_recieved == C_SET)
  {
    is_OK = TRUE;
  }
  else if (idx == 3 && byte_recieved == BCC(A_EE, C_SET))
  {
    is_OK = TRUE;
  }

  return is_OK;
}

int checkDiscEByteRecieved(unsigned char byte_recieved, int idx){
  int is_OK = FALSE;

  
  if((idx == 0 || idx == 4) && byte_recieved == FLAG){
    is_OK = TRUE;
  }
  else if (idx == 1 && byte_recieved == A_EE){
    is_OK = TRUE;
  }
  else if (idx == 2 && byte_recieved == C_DISC){
    is_OK = TRUE;
  }
  else if (idx == 3 && byte_recieved == BCC(A_EE, C_DISC)){
    is_OK = TRUE;
  }

  return is_OK;
}

int checkUA_E_ByteRecieved(unsigned char byte_recieved, int idx){
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
  int SET_received = 0;
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

int disconnect_receiver(int fd){
  STOP = FALSE;

  int DISC_received = 0;
  int res, idx = 0;

  state_receiver=DISCONNECTING; //temporário!! serve para o codigo entrar no while

  //while disconneting
  while (state_receiver==DISCONNECTING)
  {
    

    //Check se os valores são iguais aos expected -> se sim continua normalmente se não vai mudar o idx para repetir leitura
    while(!STOP){

      res = read(fd, &buf_R[idx], 1);

      if ((DISC_received == 1) && (idx == 2) && (buf_R[idx] == C_UA)){
        state_receiver=FINISHED;
      }

      if (checkDiscEByteRecieved(buf_R[idx], idx) || (checkUA_E_ByteRecieved(buf_R[idx], idx) && (state_receiver==FINISHED)))
        idx++;
      
      else{
        idx = 0; //volta ao início?
      }

      if (idx == SU_TRAMA_SIZE){
        STOP = TRUE;
        DISC_received = 1;
        //state==FINISHED;
      }

    }

    if(STOP && state_receiver!=FINISHED){
      printf(" - Received DISC...\n");
      printTramaRead(buf_R, SU_TRAMA_SIZE);

      //Send receiver disconnect
      //Envio de DISC_R
      printf(" - Sending DISC_R\n");
      if (writeData(fd, DISC_R, SU_TRAMA_SIZE) < 0)
        perror("    Error writing DISC_R\n");

      printf("\n");
    }

    STOP = FALSE;
    idx = 0;

    //se receber disconnect sai do loop
  }

  //só faz print se valor correto
  printTramaRead(buf_R, SU_TRAMA_SIZE);

  //     sleep(1);
  
  printf("    Receiver Disconnecting, Adios!...\n");

  return 0;
}

int llclose_receiver(int fd){
  if(tcsetattr(fd, TCSANOW, &oldtio) == -1) {
    perror("tcsetattr");
    return ERROR;
  }

  close(fd);
  
  return ALL_OK;
}