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


int checkDataFrame(unsigned char* frame, int Nr, int size){

    if(size < 100){
        return TRUE;
    }

    unsigned short BCC2 = 0;

    if(Nr){
        if(frame[2] != C_NS0){
            return FALSE;
        }
    }
    else{
        if(frame[2] != C_NS1){
            return FALSE;
        }
    }

    if(Nr){
        if(frame[3] != A_EE^C_NS0){
            return FALSE;
        }
    }
    else{
        if(frame[3] != 0x43){
            return FALSE;
        }
    }

    

    for(int c = 4; c < size-2; c++){
        BCC2 = BCC(frame[c], BCC2);
    }

    
    if(BCC2 != frame[size-2]){
        return FALSE;
    }

    return TRUE;
}


void handleIFrameState(char c, int* state){
    switch(*state){
        case 0:{
            if(c==FLAG) *state = 1;
            break;
        }
        case 1:{
            if(c==A_EE) *state = 2;
            else *state = 0;
            break;
        }
        case 2:{
            if(c==C_NS0 || c==C_NS1) *state = 3;
            else *state = 0;  
            break;
        }
        case 3:{
            if(c==A_EE^C_NS0 || c==A_EE^C_NS1) *state = 4;
            else *state = 0; 
            break;
        }
        case 4:{
            if(c==FLAG) *state = 5;
            break;
        }
    }
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

  
  int SET_received = 0;
  int idx = 0;
  state_receiver = CONNECTING;

  while (state_receiver == CONNECTING){
   
    printf(" - Receiving SET...\n");
    while (!STOP)
    {
      
      if((idx == 0) && (SET_received == 1)){
        clean_buf(buf_R,MAX_SIZE);
      }
      res = read(fd, &buf_R[idx], 1);

     
      if( (idx == 0) && (SET_received == 1) && (buf_R[idx] == 0x02)){ 
        idx++;
        state_receiver=TRANSFERRING;
  
        break;
      }

      if (checkSETByteRecieved(buf_R[idx], idx) == TRUE) 
        idx++;
      else idx = 0; 

      if (idx == 5){ 
        STOP = TRUE;
        state_receiver=TRANSFERRING; 
      }
    }

    if (STOP == TRUE) 
    {
    
      //printData(buf_R, SU_TRAMA_SIZE, READ);

      sleep(2);
      printf("\n");

  
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
  STOP = FALSE;

  int DISC_received = 0;
  int res, idx = 0;

  state_receiver=DISCONNECTING; 


  while (state_receiver==DISCONNECTING)
  {
    

  
    while(!STOP){

      res = read(fd, &buf_R[idx], 1);

      if ((DISC_received == 1) && (idx == 1) && (buf_R[idx] == A_ER)){
        state_receiver=FINISHED;
      }


      if (checkDiscEByteRecieved(buf_R[idx], idx) || (checkUA_E_ByteRecieved(buf_R[idx], idx) && (state_receiver==FINISHED)))
        idx++;
      
      else{
        idx = 0; 
      }

      if (idx == SU_TRAMA_SIZE){
        STOP = TRUE;
        DISC_received = 1;
    
      }

    }

    if(STOP && state_receiver!=FINISHED){
      printf(" - Received DISC...\n");
      //printData(buf_R, SU_TRAMA_SIZE, READ);

      printf(" - Sending DISC_R\n");
      if (writeData(fd, DISC_R, SU_TRAMA_SIZE) < 0)
        perror("    Error writing DISC_R\n");

      printf("\n");
    }

    STOP = FALSE;
    idx = 0;

  
  }

  printf(" - Received UA_E...\n");
  //printData(buf_R, SU_TRAMA_SIZE, READ);

  
  printf("    Receiver Disconnecting, Adios!...\n");

  sleep(2);

  if(tcsetattr(fd, TCSANOW, &oldtio) == -1) {
    perror("tcsetattr");
    return ERROR;
  }

  close(fd);
  
  return ALL_OK;
}
