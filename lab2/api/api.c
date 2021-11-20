#include "api.h"

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
        return llclose_transmitter(fd); 
    }
    else if(stat == RECEIVER){
        return llclose_receiver(fd);
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