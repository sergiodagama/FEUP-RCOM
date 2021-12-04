#include "../includes/api.h"

extern int connect_attempt;

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

int checkRRByteRecieved(unsigned char* buf, int index, int Ns){
    int is_OK = FALSE;

    if ((index == 0 && buf[0] != FLAG) || (index == 4 && buf[4] != FLAG)){
        return is_OK;        
    }
    if(index == 1 && buf[1] != A_EE){
        return is_OK;
    }
    if(index == 2 && buf[2] != C_RR_NS1 && Ns == 0){
        return is_OK;
    }
    if(index == 2 && buf[2] != C_RR_NS0 && Ns == 1){
        return is_OK;
    }
    if(index == 3 && (buf[3] != (A_EE^C_RR_NS1)) && Ns == 0){
        //printf("HERE AEE NS1\n");
        return is_OK;
    }
    if(index == 3 && (buf[3] != (A_EE^C_RR_NS0)) && Ns == 1){
        //printf("HERE AEE NS0\n");
        return is_OK;
    }

    //printf("RR good :)\n");

    is_OK = TRUE;
    return is_OK;

}


int isRej(unsigned char c, int Ns){
    if(Ns == 0){
        return c == C_REJ_NS1;
    }
    if(Ns == 1){
        return c == C_REJ_NS0;
    }
}



int Ns = 1;

int llwrite(int fd, unsigned char* buffer, int length){

    

     Ns = (Ns == 0) ? 1 : 0;
     //printf("\nNS: %d\n", Ns);

     unsigned char buf_RR[SU_TRAMA_SIZE];

    if(length > (I_FRAME_SIZE / 2) - 4){
        perror("Buffer data too big\n");
        return ERROR;
    }

    int res = 0;

    //frame creation
    unsigned char* frame = (unsigned char*) malloc(I_FRAME_SIZE);

    frame[0] = FLAG; //FLAG
    frame[1] = A_EE; //Address

    if(!Ns){
        frame[2] = C_NS0; //Control
    }else{
        frame[2] = C_NS1;
    }
    frame[3] = BCC(A_EE, frame[2]); //BCC1  TODO

    unsigned short BCC2 = 0;

    for(int i = 0; i < length; i++){  //Data
        frame[4 + i] = buffer[i];
        BCC2 = buffer[i]^BCC2;
    }

    frame[length + 4] = BCC2; //BCC2 TODO
    frame[length + 5] = FLAG; //FLAG

    //printf("BEFORE STUFFIN\n");

    //printData(frame, I_FRAME_SIZE, WRITE);



    //frame stuffing
    unsigned char* stuffed_frame = malloc(I_FRAME_SIZE);

    clean_buf(stuffed_frame, I_FRAME_SIZE);
    
    byteStuffing(length + 6, frame, stuffed_frame);

    free(frame);

    //write frame
    int idx;
    //signal(SIGALRM, atende);  // instala a rotina que atende interrupcao
    //siginterrupt(SIGALRM, 1); // quando o sinal SIGALRM é apanhado, provoca uma interrupção no read()

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

       //printf("AFTER STUFFIN\n");
        
       //printData(stuffed_frame, I_FRAME_SIZE, WRITE);

      
        printf("\n");
        
        idx = 0;
        alarm(ALARM_SECONDS);
        int good_rr = TRUE;

        int timedout = 0;
        
        

        //wait for response RR /REJ

        while (!STOP)
        { /* loop for input */
            //sleep(1);
            if(idx == 0){
                clean_buf(buf_RR, SU_TRAMA_SIZE);
            }
            //printf("before read\n");
            if( (res = read(fd, &buf_RR[idx], 1)) < 0){
                if (errno == EINTR){
                    timedout = 1;
                    printf("    Timed Out attempt: \n\n");
                    break;
                }
                else{
                    perror("    Read failed\n\n");

                }
            }
            
            if(idx == 2){
             if (isRej(buf_RR[2], Ns)){ good_rr == FALSE; idx++;}
             else if (checkRRByteRecieved(buf_RR, idx, Ns)) {idx++;}
             else idx = 0;
            }

            else if (checkRRByteRecieved(buf_RR, idx, Ns)){ //verifica se está a receber os bytes do SET corretos
                idx++;
                //idx = 0; //nao seria voltar a enviar data? inves de recber RR again
                //printf("RR byte (%d) not good :(\n", idx);
            }
            else
                idx = 0;


            if (idx == 5){ 
                STOP = TRUE;
            }
        }
        alarm(0);
        
        if(timedout == 1){
            printf("Timedout while reading RR/REJ \n");
        }
        
        else if(isRej(buf_RR[2], Ns) || !good_rr){  //if it is rej or not good rr go back and send again 
            printf(" - Received REJ...\n");
            printData(buf_RR, SU_TRAMA_SIZE, READ);

            connect_attempt = 1;
        }
        else if (good_rr && timedout == 0) {
            SENDING = FALSE;
        }
    }

    free(stuffed_frame);


    if (STOP == TRUE)
    {
        printf(" - Received RR...\n");
        //só faz print se valor correto
        printData(buf_RR, SU_TRAMA_SIZE, READ);
    }


    //printf("RES: %d\n", res);
    return res;
}








int checkDataFrame(unsigned char* frame, int Nr, int size){

    //calculating BCC2
    unsigned short BCC2 = 0;

    

    //printf("LAST FLAG: %d\n", last_flag);

    for(int c = 4; c < size-2; c++){
        BCC2 = BCC(frame[c], BCC2);
    }

    
    if(BCC2 != frame[size-2]){
        printf("HEREEEE\n");
        return FALSE;
    }

    return TRUE;
}


void handleIFrameState(char c, int* state, int Nr){
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
            if(Nr){
                if(c==C_NS0) *state = 3;
                else *state = 0;  
            }
            else{ 
                if(c==C_NS1) *state = 3;
                else *state = 0; 
            } 
            
            break;
        }

        case 3:{
            if(Nr){
                if(c==A_EE^C_NS0) *state = 4;
                else *state = 0; 
            }
            else{
                if(c==A_EE^C_NS1) *state = 4;
                else *state = 0; 
            }
            
            break;
        }

        case 4:{
            if(c==FLAG) *state = 5;
            break;
        }
    }
}

int Nr = 0;

int llread(int fd, unsigned char* buffer){

    Nr = (Nr == 0) ? 1 : 0; // change to be depdent on Ns received! TODO

    //printf("\nNR: %d\n", Nr);

    int res = 0, index = 0; //<- numero de bytes,
    int stage = 0, STOP = FALSE;

    
    //printf("A\n");
    unsigned char* frame; //= malloc(I_FRAME_SIZE);
    //printf("B\n");
    unsigned char* original; // = malloc(I_FRAME_SIZE);
    //printf("C\n");
    unsigned char c;
    //printf("D\n");
    frame = malloc(I_FRAME_SIZE);
    original = malloc(I_FRAME_SIZE);
    

    while(!STOP){
        //reading frame
    

        //printf("\tIm on loop llread\n");

        while (stage <5) { 
    
            //printf("Before read inside llread %d\n", index);
            res += read(fd, &c, 1);
            //printf("After read inside llread %d - read %c\n", index, c);

            frame[index] = c;
            

            handleIFrameState(frame[index], &stage, Nr);

            if(stage != 0) index++;
            else index = 0;

        }
        //printf("Before Reverse Stuffing: \n");
        //printData(frame, I_FRAME_SIZE, READ);

        //reverse the byte stuffing
        reverseByteStuffing(&index, frame, original);
        printf("Before Free Frame\n\n");
        //free(frame);
        printf("After Free Frame\n\n");

        //printf("After Reverse Stuffing: \n");
        //printData(original, I_FRAME_SIZE, READ);

        //checking read frame
        if (checkDataFrame(original, Nr, index)){  
            STOP = TRUE;
            printf("\n DATA ALL OK - bytes read: %d\n", res);
             for (int q = 0; q < index; q++) {
                buffer[q] = original[q];
             }
        }
        else{
            //sending REJ
            printf(" Data frame wrong - Sending REJ\n");  //if rej go back
            if(Nr == 1){
                if (writeData(fd, RJ1, SU_TRAMA_SIZE) < 0)
                    perror("    Error writing RJ1\n");
            }
            else if(Nr == 0){
                if (writeData(fd, RJ0, SU_TRAMA_SIZE) < 0)
                    perror("    Error writing RJ0\n");
            }

            //clean frame and reset variables
            
            //sleep(1);

            clean_buf(frame, I_FRAME_SIZE);
            clean_buf(original, I_FRAME_SIZE);
            stage = 0;
            res = 0;
            index = 0;
        }
        //free(original);
    }
    free(original);
    free(frame);
    //printf("C\n");
     
    if (STOP == TRUE) { //se recebeu o I Frame corretamente, envia o RR ou REJ para o Transmitter  (MISSING REJ)

      //sleep(2);
      printf("\n");

      //Envio de RR
      printf(" - Sending RR\n");
      if(Nr == 1){
        if (writeData(fd, RR1, SU_TRAMA_SIZE) < 0)
            perror("    Error writing RR1\n");
      }
      else if(Nr == 0){
          if (writeData(fd, RR0, SU_TRAMA_SIZE) < 0)
            perror("    Error writing RR0\n");
      }
        

      printf("\nAll OK on receiver!\n");
    }

    return 0;
}
