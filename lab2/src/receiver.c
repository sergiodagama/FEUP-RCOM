#include "../includes/receiver.h"

/**
 * @brief Receiver application layer main function
 * 
 * @param argc number of arguments passed to receiver
 * @param argv the actual arguments -> ./[program_name] [port_file_name]
 * @return negatice value in case of error, 0 otherwise 
 */

int main(int argc, char** argv){

    //checking for user input error
    if(argc < 2){
        perror("Input error: too few arguments\n");
        printf("Usage:\t[port_file_name]\tex: /dev/ttyS1\n");
        return ERROR;
    }

    printf("\n---------CONNECTING---------\n");

    int fd = ERROR; //port file descriptor
    
    //filling application layer info
    ApplicationLayer receiver;
    receiver.status = RECEIVER;

    //openning connection
    if(llopen(argv[1], receiver.status, &fd) < 0){
        perror("Error: receiver llopen function call\n");
        return ERROR;
    }

    //checking errors in file descriptor
    if(fd != ERROR)
        receiver.fileDescriptor = fd;
    else{
        perror("Error: file descriptor not updated\n");
        return ERROR;
    }

    printf("\n----------CONNECTED-----------\n");

    
    printf("\n--------RECEIVING DATA--------\n");

    //receiving data TODO

    ////receive start packet

    ////loop to receive data

    ////receive end packet 

    printf("\n----------ALL DATA SENT----------\n\n");
    
    printf("\n----------DISCONNECTING----------\n\n");

    //closing connection
    if(llclose(receiver.fileDescriptor, receiver.status) < 0){
      perror("Error: receiver llclose function call\n");
      return ERROR;
    }

    printf("\n----------DISCONNECTED-----------\n\n");
    

    return 0;
}