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

int I_FRAME_SIZE = 20;  //4 bytes for header and 90 max for data + 2 bytes for header [96 max total before stuffing] 20 to be changed to 200?


int checkRRByteRecieved(unsigned char byte_recieved, int idx, int Ns){
    return TRUE;
}

int llwrite(int fd, char* buffer, int length, int Ns){
     unsigned char buf_RR[MAX_SIZE];

    if(length > (I_FRAME_SIZE / 2) - 4){
        perror("Buffer data too big\n");
        return ERROR;
    }

    int res = 0;

    //frame creation
    unsigned char frame[I_FRAME_SIZE];

    frame[0] = FLAG; //FLAG
    frame[1] = A_EE; //Address

    if(!Ns){
        frame[2] = C_NS0; //Control
    }else{
        frame[2] = C_NS1;
    }
    frame[3] = BCC(A_EE, C_NS0); //BCC1  TODO

    unsigned short BCC2 = 0;

    for(int i = 0; i < length; i++){  //Data
        frame[4 + i] = buffer[i];
        BCC2 = BCC(buffer[i], BCC2);
    }

    frame[length + 4] = BCC2; //BCC2 TODO
    frame[length + 5] = FLAG; //FLAG

    //frame stuffing
    unsigned char* stuffed_frame = malloc(I_FRAME_SIZE);

    stuffed_frame = byteStuffing(length + 6, frame);

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
        
        if((res = write(fd, stuffed_frame, I_FRAME_SIZE)) < 0){ 
            perror("    Error writing DATA\n");
        }
        printTramaRead(stuffed_frame, I_FRAME_SIZE);

        printf("\n");
        
        idx = 0;
        alarm(ALARM_SECONDS);
        flag = 0;

        alarm(0); //Reset alarm

        int rr_received = 0; 
        //wait for response RR /REJ
        printf(" - Receiving RR or RJ...\n");

        while (!STOP) {

            if((idx == 0) && (rr_received == 1)){
                clean_buf(buf_RR, MAX_SIZE);
            }
            res = read(fd, &buf_RR[idx], 1);

            if (checkRRByteRecieved(buf_RR[idx], idx, Ns) == TRUE) //verifica se está a receber os bytes do RR corretos   TODO
                idx++;
            else idx = 0; //volta ao início?

            if (idx == 5){ 
                STOP = TRUE;
                SENDING = FALSE;
            }
        }
    }

    if (STOP == TRUE) //se recebeu o SET corretamente, envia o UA para o Transmitter
    {
        //só faz print se valor correto
        printTramaRead(buf_RR, SU_TRAMA_SIZE);
    }

    printf("RES: %d\n", res);
    return res;
}

int checkDataFrame(unsigned char* frame, int Nr){
    int is_OK = FALSE;

    if (frame[0] == FLAG){
        is_OK = TRUE;
    }
    else if (frame[1] == A_EE){
        is_OK = TRUE;
    }
    else if (frame[2] != Nr){ //to be change!!
        is_OK = TRUE;
    }
    else if (frame[3] == BCC(A_EE, (!Nr))){
        is_OK = TRUE;
    }
    //MISSING REST OF CHECKING

    return is_OK;
}

int llread(int fd, char* buffer, int Nr){
    int res = 0, index = 0, stage = 0, STOP = FALSE;

    unsigned char frame[I_FRAME_SIZE];
    unsigned char c;

    while(!STOP){
        //reading frame
        while (stage < 2) { 
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
        }
        printf("Before Reverse Stuffing: \n");
        printTramaRead(frame, I_FRAME_SIZE);

        //reverse the byte stuffing
        unsigned char* original = malloc(I_FRAME_SIZE);
        original = reverseByteStuffing(I_FRAME_SIZE, frame);

        //checking read frame
        if (checkDataFrame(frame, Nr)){  //TODO FUNCTION NOT FINISHED
            STOP = TRUE;

            printf("\nBYTES READ: %d\n", res);

            printf("After Reverse Stuffing: \n");
            printTramaRead(original, I_FRAME_SIZE);
        }
        else{
            //send REJ here ?

            //clean frame and reset variables
            clean_buf(frame, I_FRAME_SIZE);
            clean_buf(original, I_FRAME_SIZE);
            stage = 0;
            res = 0;
            index = 0;
        }
    }
     
    if (STOP == TRUE) { //se recebeu o I Frame corretamente, envia o RR ou REJ para o Transmitter  (MISSING REJ)

      sleep(2);
      printf("\n");

      //Envio de RR
      printf(" - Sending RR\n");
      if(Nr == 1){
        if (writeData(fd, RR1, SU_TRAMA_SIZE) < 0)
            perror("    Error writing UA\n");
      }
      else if(Nr == 0){
          if (writeData(fd, RR0, SU_TRAMA_SIZE) < 0)
            perror("    Error writing UA\n");
      }
        

      printf("\nAll OK on receiver!\n");
    }

    return 0;
}
