/*Non-Canonical Input Processing*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>

#include "macrosLD.h"
#include "utils.c"
#include "api.c"

#define BAUDRATE B38400
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

volatile int STOP = FALSE;

unsigned char buf_R[MAX_SIZE];

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

int main(int argc, char **argv)
{
  
  //llopen beginnig (open_reader)
  int fd, c, res;

  enum state state;
  
  if (argc > 2){
      perror("Too many arguments\n");
      return 1;
  }
    
  if ((fd = llopen(argv[1], RECEIVER)) < 0){
    perror("llopen error\n");
    return 1;
  }

  int idx = 0;
  state = CONNECTING;

  int SET_received = 0;

  //while - open connection
  while (state == CONNECTING)
  {

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
        state==TRANSFERRING;
        //receive_control_packet(buf_R, fd, ...);
        break;
      }

      if (checkSETByteRecieved(buf_R[idx], idx) == TRUE) //verifica se está a receber os bytes do SET corretos
        idx++;
      else idx = 0; //volta ao início?

      if (idx == 5){ 
        STOP = TRUE;
        state=TRANSFERRING; //com isto domentado o programa corre, porém na versão final tem q esta descomentado
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

  //receiving data


  // while(state==TRANSFERRING){

  //   res = read(fd, &buf_R[idx], 1);

  //   if((idx == 0) && (buf_R[idx] == 0x03)){ //verificar se está a receber um packet de controlo END
  //     state==DISCONNECTING;
  //     //receive_control_packet(buf_R, fd);
  //     break;
  //   }

  //   //guardar a trama de informação em algum lado

  //   //responder transmitor



    
  // }

  STOP = FALSE;

  int DISC_received = 0;
  idx = 0;

  state=DISCONNECTING; //temporário!! serve para o codigo entrar no while

  //while disconneting
  while (state==DISCONNECTING)
  {
    

    //Check se os valores são iguais aos expected -> se sim continua normalmente se não vai mudar o idx para repetir leitura
    while(!STOP){

      res = read(fd, &buf_R[idx], 1);

      if ((DISC_received == 1) && (idx == 2) && (buf_R[idx] == C_UA)){
        state=FINISHED;
      }

      if (checkDiscEByteRecieved(buf_R[idx], idx) || (checkUA_E_ByteRecieved(buf_R[idx], idx) && (state==FINISHED)))
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

    if(STOP && state!=FINISHED){
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

  llclose(fd, RECEIVER);

  return 0;
}
