#include "../includes/app_receiver.h"

/**
 * @brief Receiver application layer main function
 * 
 * @param argc number of arguments passed to receiver
 * @param argv the actual arguments -> ./[program_name] [port_file_name]
 * @return negatice value in case of error, 0 otherwise 
 */

int main(int argc, char** argv){

    // Checking for user input error
    if(argc < 2){
        perror("Input error: too few arguments\n");
        printf("Usage:\t[port_file_name]\tex: /dev/ttyS11\n");
        return ERROR;
    }

    printf("\n########### RECEIVER ##########\n");

    printf("\n-----------CONNECTING----------\n\n");

    int fd = ERROR; //port file descriptor
    
    ApplicationLayer receiver;
    receiver.status = RECEIVER;

    if(llopen(argv[1], receiver.status, &fd) < 0){
        perror("Error: receiver llopen function call\n");
        return ERROR;
    }

    if(fd != ERROR)
        receiver.fileDescriptor = fd;
    else{
        perror("Error: file descriptor not updated\n");
        return ERROR;
    }

    printf("\n----------CONNECTED-----------\n");

    printf("\n--------RECEIVING DATA--------\n");

    unsigned long file_size = 0;
    unsigned char file_name[1000];
    unsigned char* name = malloc(100);
     
    strcpy(file_name, "files/receive/");


    
    file_size = receiveStartPacket(fd, name);

    strcat(file_name, name); 

    FILE *file_fd1;

    if((file_fd1 = fopen(file_name, "w")) == NULL){
        perror("Error opening file to send\n");
        return ERROR;
    }

    
    unsigned char *packet = malloc(I_FRAME_SIZE); 
    

    unsigned char* data = malloc(DATA_SIZE);
    int idx = 0;
    int NOT_END = TRUE;
    
    while(NOT_END){

        clean_buf(packet, I_FRAME_SIZE);
        clean_buf(data, DATA_SIZE);

    
        llread(fd, packet);
       

        if(checkControlPacket(END, packet) == TRUE){
            NOT_END = FALSE;
            break;
        }

        for(int i = 0; i < DATA_SIZE; i++){
            
            data[i] = packet[i+4];
            
        }

        
        fwrite(data, 1, DATA_SIZE, file_fd1);

        idx++;
    }


    close(file_fd1);
    
    printf("\n--------ALL DATA RECEIVED--------\n\n");
    
    printf("\n----------DISCONNECTING----------\n\n");

   
    if(llclose(receiver.fileDescriptor, receiver.status) < 0){
        perror("Error: receiver llclose function call\n");
        return ERROR;
    }

    printf("\n----------DISCONNECTED-----------\n\n");
    

    return 0;
}