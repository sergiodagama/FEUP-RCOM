#include "../includes/api.h"

int llopen(char* port, enum status stat, int* fid){
    if(stat == TRANSMITTER){
        if ((strcmp("/dev/ttyS0", port)!=0) && 
            (strcmp("/dev/ttyS1", port)!=0) && 
            (strcmp("/dev/ttyS10", port)!=0)) {
                printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS10\n");
                return ERROR;
        }
        return llopen_transmitter(port, fid);
    }
    
    else if(stat == RECEIVER){
        if ((strcmp("/dev/ttyS0", port)!=0) && 
            (strcmp("/dev/ttyS1", port)!=0) && 
            (strcmp("/dev/ttyS11", port)!=0)){
                printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS11\n");
                return ERROR;
        }
        return llopen_receiver(port, fid);
    }
    else{
        perror("\nError: unknown status on llopen function call\n");
        return ERROR;
    }
}

int llclose(int fd, enum status stat){
    if(stat == TRANSMITTER){
        if(llclose_transmitter(fd) <0){
            printf("Failed to disconnect transmitter\n");
            return ERROR;
        }
        return 0;
        
    }
    else if(stat == RECEIVER){
        if(llclose_receiver(fd) < 0){
            printf("Failed to disconnect receiver\n");
            return ERROR;
        }
        return 0;
    }
    else{
        perror("\nError: unknown status on llclose function call\n");
        return ERROR;
    }
}

/*
I frames structure:
    F Flag
    A Address field
    C Control field
    D1..Dn Data field
    BCC1, BCC2 Protection fields (1 - header, 2 - data)
*/

int I_FRAME_SIZE = 200;  //4 bytes for header and 90 max for data + 2 bytes for header [96 max total]-> after stuffing 200 (8 left)

int llwrite(int fd, char* buffer, int length, int Ns){

    if(length > 90){
        perror("Buffer data too big\n");
        return ERROR;
    }

    int res = 0;

    //frame creation
    unsigned char frame[100];

    frame[0] = FLAG; //FLAG
    frame[1] = A_EE; //Address
    if(!Ns){
        frame[2] = C_NS0; //Control
    }else{
        frame[2] = C_NS1;
    }
    frame[3] = 0; //BCC1  TODO

    for(int i = 0; i < length; i++){  //Data
        frame[4 + i] = buffer[i];
    }

    frame[length + 4] = 0; //BCC2 TODO
    frame[length + 5] = FLAG; //FLAG

    //frame stuffing
    unsigned char stuffed_frame[I_FRAME_SIZE];

    //stuffed_frame = byte_stuffing(length + 6, frame);

    //write frame
    signal(SIGALRM, atende);  // instala a rotina que atende interrupcao
    siginterrupt(SIGALRM, 1); // quando o sinal SIGALRM é apanhado, provoca uma interrupção no read()
    int idx;

    connect_attempt = 1;
  
    int SENDING = TRUE, STOP = FALSE;

    while(SENDING){
        if (connect_attempt > MAX_ATTEMPS){
            printf("Sender gave up, attempts exceded\n");
            return ERROR;
        }

        printf("Attempt %d\n - Sending DATA frame\n", connect_attempt);
        
        if((res = write(fd, frame, 100)) < 0){  //change to I_FRAME_SIZE and suffed_frame when implemented bytestuffing
            perror("    Error writing DATA\n");
        }

        printf("\n");
        

        //wait for response RR /REJ
        idx = 0;
        alarm(ALARM_SECONDS);
        flag = 0;

        alarm(0); //Reset alarm

        printf(" - Receiving RR or RJ...\n");
/*
        while (!STOP) {
            
            
        }*/
        break;
    }
    printf("RES: %d\n", res);
    return res;
}

int llread(int fd, char* buffer, int Nr){

    int res = 0;
    int index = 0;

    unsigned char frame[100];

    unsigned char c;

    int stage = 0;


    while (stage < 2) { 
        //printf("reading...\n");
        res += read(fd, &c, 1);


      if(c == FLAG && stage == 0){  //Found final flag
        stage = 1;
      }
      else if(c==FLAG && stage == 1){
          stage = 2;
      }


      if(stage > 0){
          frame[index] = c;
          index++;
      }


      //if (checkDataFrame()){ 
       // STOP = TRUE;
      //}
      //if it is valid take data out of frame and fill buffer
      
    }

    printf("\nBYTES READ: %d\n\n", res);
    printTramaRead(frame, index);
    
     
    
    
    
/*
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
    }*/
    return 0;
}
