#include "../includes/api.h"

//#include "byte_stuffing.h"

int llopen(char* port, enum status stat, int* fid){
    if(stat == TRANSMITTER){
        if ((strcmp("/dev/ttyS0", port)!=0) && 
            (strcmp("/dev/ttyS1", port)!=0) && 
            (strcmp("/dev/ttyS10", port)!=0)) {
                printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
                return ERROR;
        }
        return llopen_transmitter(port, fid);
    }
    
    else if(stat == RECEIVER){
        if ((strcmp("/dev/ttyS0", port)!=0) && 
            (strcmp("/dev/ttyS1", port)!=0) && 
            (strcmp("/dev/ttyS11", port)!=0)){
                printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
                return ERROR;
        }
        return llopen_receiver(port, fid);
    }
    else{
        perror("\nError: unknown status on llopen call\n");
        return ERROR;
    }
}

int llclose(int fd, enum status stat){
    if(stat == TRANSMITTER){
        if (disconnect_transmitter(fd) < 0){
            printf("Failed to disconnect transmitter\n");
            return ERROR;
        }
        if(llclose_transmitter(fd) <0){
            printf("Failed to close transmitter\n");
            return ERROR;
        }
        return 0;
        
    }
    else if(stat == RECEIVER){
        if (disconnect_receiver(fd) < 0){
            printf("Failed to disconnect transmitter\n");
            return ERROR;
        }
        if(llclose_receiver(fd) < 0){
            printf("Failed to close transmitter\n");
            return ERROR;
        }
        return 0;
    }
    else{
        perror("\nError: unknown status on llopen call\n");
        return ERROR;
    }
}

/*
int llwrite(int fd, char* buffer, int length){
    //TODO
}

int llread(int fd, char* buffer){
    //TODO
}
*/