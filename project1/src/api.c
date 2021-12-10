#include "../includes/api.h"

extern int connect_attempt;

int Nr = 0;
int Ns = 1;

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




int llwrite(int fd, unsigned char* buffer, int length){

    Ns = (Ns == 0) ? 1 : 0;
    unsigned char buf_RR[SU_TRAMA_SIZE];

    if(length > (I_FRAME_SIZE / 2) - 4){
        perror("Buffer data too big\n");
        return ERROR;
    }

    int res = 0;

 
    unsigned char* frame = (unsigned char*) malloc(I_FRAME_SIZE);

    frame[0] = FLAG; 
    frame[1] = A_EE; 

    if(!Ns){
        frame[2] = C_NS0; 
    }else{
        frame[2] = C_NS1;
    }
    frame[3] = BCC(A_EE, frame[2]); 

    unsigned short BCC2 = 0;

    for(int i = 0; i < length; i++){  
        frame[4 + i] = buffer[i];
        BCC2 = buffer[i]^BCC2;
    }

    frame[length + 4] = BCC2;
    frame[length + 5] = FLAG; 



    unsigned char* stuffed_frame = malloc(I_FRAME_SIZE);

    clean_buf(stuffed_frame, I_FRAME_SIZE);
    
    byteStuffing(length + 6, frame, stuffed_frame);

    free(frame);


    int idx;

    connect_attempt = 1;

    int timedout, STOP;
  
    int SENDING = TRUE;

    while(SENDING){
        if (connect_attempt > MAX_ATTEMPS){
            printf("Sender gave up, attempts exceded\n");
            return ERROR;
        }

        printf("Attempt %d\n - Sending DATA frame\n", connect_attempt);
        
        
        if((res = write(fd, stuffed_frame, I_FRAME_SIZE)) < 0){ 
            perror("    Error writing DATA\n");
        }


      
        printf("\n");
        
        idx = 0;
        alarm(ALARM_SECONDS);
        int good_rr = TRUE;
        STOP = FALSE;

        timedout = 0;
        


        while (!STOP)
        {

            if( (res = read(fd, &buf_RR[idx], 1)) < 0){
                if (errno == EINTR){
                    timedout = 1;
                    break;
                }
                else{
                    perror("    Read failed\n\n");

                }
            }

            
            if(idx == 2 || idx == 3){
                
                if (isRej(buf_RR[idx],idx, Ns) == TRUE){
                    good_rr = FALSE; 
                    idx++;
                }

                else if (checkRRByteRecieved(buf_RR, idx, Ns) == TRUE){idx++;}
                else idx = 0;
            }

            else if (checkRRByteRecieved(buf_RR, idx, Ns) == TRUE){ 
                idx++;
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
        
        else if(!good_rr){  
            printf(" - Received REJ...\n");
            printData(buf_RR, SU_TRAMA_SIZE, READ);
            connect_attempt++;
        }
        else if (good_rr) {
            SENDING = FALSE;
  
        }

        clean_buf(buf_RR, 5);
    }

    free(stuffed_frame);


    if (STOP == TRUE)
    {
        printf(" - Received RR...\n"); 
        printData(buf_RR, SU_TRAMA_SIZE, READ);
    }

    return res;
}




int llread(int fd, unsigned char* buffer, unsigned int delay, unsigned int GenErrors){

    unsigned int randNum;

    Nr = (Nr == 0) ? 1 : 0; 

    int res = 0, index = 0; 
    int stage = 0, STOP = FALSE;

    
    unsigned char* frame;
    unsigned char* original;
    unsigned char c;

    frame = malloc(I_FRAME_SIZE);
    original = malloc(I_FRAME_SIZE);

    int errorCreation;
    

    while(!STOP){
        errorCreation = 0;

        randNum = rand() % 1000;

        if(randNum < GenErrors){
            errorCreation = 1;
        }

        while (stage <5) { 
    
            res += read(fd, &c, 1);

            frame[index] = c;

            handleIFrameState(frame[index], &stage);

            if(stage != 0) index++;
            else index = 0;

        }

        sleep(delay);

        reverseByteStuffing(&index, frame, original);

        if(errorCreation==1){
            original[index-2] = 0xFF - original[index-2];
        }

        if (checkDataFrame(original, Nr, index)){  
            STOP = TRUE;
            printf("\n DATA ALL OK - bytes read: %d\n", res);
             for (int q = 0; q < index; q++) {
                buffer[q] = original[q];
             }
        }
        else{
            printf(" Data frame wrong - Sending REJ\n");  
            if(Nr == 1){
                if (writeData(fd, RJ1, SU_TRAMA_SIZE) < 0)
                    perror("    Error writing RJ1\n");
            }
            else if(Nr == 0){
                if (writeData(fd, RJ0, SU_TRAMA_SIZE) < 0)
                    perror("    Error writing RJ0\n");
            }

            clean_buf(frame, I_FRAME_SIZE);
            clean_buf(original, I_FRAME_SIZE);
            stage = 0;
            res = 0;
            index = 0;
        }
    }
    free(original);
    free(frame);

     
    if (STOP == TRUE) { 

      printf("\n");

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
