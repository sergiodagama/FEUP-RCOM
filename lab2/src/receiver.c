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
        printf("Usage:\t[port_file_name]\tex: /dev/ttyS11\n");
        return ERROR;
    }

    printf("\n########### RECEIVER ##########\n");

     printf("\n-----------CONNECTING----------\n\n");

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
    FILE *file_fd1;

    if((file_fd1 = fopen("src/pinguim2.gif", "wb")) == NULL){
      perror("Error opening file to send\n");
      return ERROR;
    }

    ////loop to receive data
    unsigned char *packet = malloc(I_FRAME_SIZE);
    unsigned char* full_data = malloc(10968);

    unsigned char* data = malloc(1024);
    int idx = 0;
    
    while(1){
        if(idx == 11) break;

        llread(fd, packet);

        for(int i = 0; i < 1024; i++){
            if(i < 4)
                continue;
            else{
                data[i-4] = packet[i];
            }
        }

       for(int j = 0; j < 1024; j++){
           full_data[(1024*idx)+j] = data[j];
           
       }
       

        idx++;
       
        ////receive end packet 
    }
    


    /*llread(fd, packet);

    printf("HEREEEE BROOOO\n");
    printData(packet, I_FRAME_SIZE, 1);


       for(int j = 0; j < 50; j++){
           full_data[j] = data[j];
           
       }
       */

    fwrite(full_data, sizeof (unsigned char), 10968, file_fd1); 
    

    printf("\n--------ALL DATA RECEIVED--------\n\n");
    
    printf("\n----------DISCONNECTING----------\n\n");

    //closing connection
    if(llclose(receiver.fileDescriptor, receiver.status) < 0){
      perror("Error: receiver llclose function call\n");
      return ERROR;
    }

    printf("\n----------DISCONNECTED-----------\n\n");
    

    return 0;
}