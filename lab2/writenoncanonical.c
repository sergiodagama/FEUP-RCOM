/*Non-Canonical Input Processing*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>


#include "alarme.c"
#include "utils.c"
#include "api.c"
#include "file.c"

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
    int fd, c, res;
    FILE *img_fp;
    img_info file;


    enum state state;

    printf("\n----------TRANSMITTER----------\n\n");

    if (argc > 2){
      perror("Too many arguments\n");
      return 1;
    }

    if (argc < 2){
      perror("Too few arguments\n");
      printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
      return 1;
    }
    
    if ((fd = llopen(argv[1], TRANSMITTER)) < 0){
      perror("llopen error\n");
      return 1;
    }


    if( (img_fp = fopen("pinguim.gif", "rb")) <0){
      printf("Error opening img\n");
      return 1;
    }
    file.name = "pinguim.gif";
    
    if(get_file_size(&file, img_fp)<0){ //preenche o file->size
      return 1;
    }

    if(read_file(&file, img_fp)<0){//preenche o file->data
      return 1;
    }



    //criar control packet de START

    //criar control packet de END

    signal(SIGALRM, atende);  // instala a rotina que atende interrupcao
    siginterrupt(SIGALRM, 1); // quando o sinal SIGALRM é apanhado, provoca uma interrupção no read()
    
    int idx;

    connect_attempt = 1;

    state = CONNECTING;

    printf("\n----------CONNECTING----------\n\n");

    //while - open connection
    while(state == CONNECTING){

      if (connect_attempt > MAX_ATTEMPS){
        printf("Sender gave up, attempts exceded\n");
        llclose(fd, TRANSMITTER);
        return 1;
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
        else  idx = 0; //volta ao início?
        
        if (idx == 5){
          STOP = TRUE;
          state = TRANSFERRING;
        }
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

    printf("\n----------CONNECTED----------\n\n");

    printf("\n---------SENDING DATA---------\n\n");

    struct DataPacket {
      unsigned char  C; //control value -> 1 for data
      unsigned char  N;  //sequence number (module 255)
      unsigned char L1;  // number of (k) data bytes (k = 256 * L2 + L1)
      unsigned char L2;
      unsigned char *data;  //data bytes
    } DataPacket; 

    struct ControlPacket {
      unsigned char  C; //control value -> 2 for start, 3 for end
      unsigned char T1;  //type of 0 -> file size, 1 -> file name
      unsigned char L1;  //number of bytes of V field
      unsigned char V1[];
      unsigned char T2;  //type of 0 -> file size, 1 -> file name
      unsigned char L2;  //number of bytes of V field
      unsigned char V2[];
    } ControlPacket;

    ControlPacket start;

    start.C = 2;
    start.T1 = 0;
    start.L1 = sizeof(img_info.name);
    start.V1 = img_info.name;
    start.T1 = 1;
    start.L1 = sizeof(img_info.size);
    start.V1 = img_info.size; //convertion to be checked

    ControlPacket end;  //start and end are equal besides the C value?

    start.C = 3;
    start.T1 = 0;
    start.L1 = sizeof(img_info.name);
    start.V1 = img_info.name;
    start.T1 = 1;
    start.L1 = sizeof(img_info.size);
    start.V1 = img_info.size; //convertion to be checked

    struct TramaI {
      unsigned char  F; 
      unsigned char A;  
      unsigned char C;  
      unsigned char BCC1;
      unsigned char data[];  
      unsigned char BCC2; 
      unsigned char F;
    } TramaI;

    index=0;
    //while - data transmission
     while(state == TRANSFERRING){

       if (connect_attempt > MAX_ATTEMPS){
         printf("Sender gave up, attempts exceded\n");
         return 1;
       }

       if(writeData(fd, start, sizeof(start));
         perror("Error writing start packet\n");

         while(SENDING_DATA){
           DataPacket packet;

           packet.c = 1;
           packet.N = index;
           packet.data = getData(img_info.data, 90, index);   //TODO
          //packet.L1
           //packet.L2
           

           index++;
         }


       if(writeData(fd, end, sizeof(end));
         perror("Error writing end packet\n");

    }

    printf("\n----------ALL DATA SENT----------\n\n");

    printf("\n----------DISCONNECTING----------\n\n");

    //!!!!!!!!!!!!!!!!!!!!NEWWWWWW!!!!!!!!!!!!!!!!!!!!!!!!
    //while - disconnect
    STOP = FALSE;

    connect_attempt = 1;

    //starting to disconnect

    state=DISCONNECTING; //temporário!! serve para o codigo entrar no while

    while(state==DISCONNECTING){

      if(connect_attempt > 4){
        printf("    Attempt %d", connect_attempt);
        llclose(fd, TRANSMITTER);
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
          state = TRANSFERRING;
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
    //!!!!!!!!!!!!!!!!!!!!NEWWWWWW!!!!!!!!!!!!!!!!!!!!!!!!

    llclose(fd, TRANSMITTER);
    free(file.data);
    return 0;
}
