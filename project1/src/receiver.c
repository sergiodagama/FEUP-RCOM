#include "../includes/receiver.h"

int checkControlPacket(enum packet_id id, unsigned char* frame){

    if(id == START){
        if(frame[4] == CP_START){
            return TRUE;
        }
         
        else{ 
            return FALSE;
        }
    }
    if(id == END){
        if(frame[4] == CP_END) return TRUE;
        else return FALSE;
    }
}

unsigned long receiveStartPacket(int fd, unsigned char* name){
    unsigned char* packet = malloc(I_FRAME_SIZE);

    unsigned long file_size = 10968;  //change to zero when all implemented

    while(TRUE){
        llread(fd, packet);
        
        //printData(packet, I_FRAME_SIZE, 1);

        if(checkControlPacket(START, packet)){
            break;
        } 
    }

    // Parsing start packet TODO

    return file_size; 
}
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
    
    // Filling application layer info
    ApplicationLayer receiver;
    receiver.status = RECEIVER;

    // Openning connection
    if(llopen(argv[1], receiver.status, &fd) < 0){
        perror("Error: receiver llopen function call\n");
        return ERROR;
    }

    // Checking errors in file descriptor
    if(fd != ERROR)
        receiver.fileDescriptor = fd;
    else{
        perror("Error: file descriptor not updated\n");
        return ERROR;
    }

    printf("\n----------CONNECTED-----------\n");

    printf("\n--------RECEIVING DATA--------\n");

    unsigned long file_size = 10968;
    char file_name[1000];
    char* name = malloc(100);
     
    strcpy(file_name, "files/receive/");


    // Receive start control packet
    
    file_size = receiveStartPacket(fd, name);

    strcat(file_name, "pinguim.gif");  //change to start packet info

    // Creating received file
    FILE *file_fd1;

    if((file_fd1 = fopen(file_name, "wb")) == NULL){
        perror("Error opening file to send\n");
        return ERROR;
    }

    // Receiving data packets
    unsigned char *packet = malloc(I_FRAME_SIZE);  //because it is the original frame data reaches here, so no need to double the memory
    unsigned char* full_data = malloc(file_size);

    unsigned char* data = malloc(DATA_SIZE);
    int idx = 0;
    int NOT_END = TRUE;
    
    while(NOT_END){
        if(idx > (file_size/DATA_SIZE)){
            NOT_END = FALSE;
            break;
        }

        clean_buf(packet, I_FRAME_SIZE);
        clean_buf(data, DATA_SIZE);

        llread(fd, packet);

        //checkControlPacket();

        for(int i = 0; i < DATA_SIZE; i++){
            data[i] = packet[i+4];
        }

        for(int j = 0; j < DATA_SIZE; j++){
            full_data[DATA_SIZE*idx + j] = data[j];

        }
        idx++;
    }

    // Receiving end packet
    clean_buf(packet, I_FRAME_SIZE);
    llread(fd, packet);
    checkControlPacket(END, packet);

    // Writing received data to file
    fwrite(full_data, sizeof (unsigned char), 10968, file_fd1); 
    
    printf("\n--------ALL DATA RECEIVED--------\n\n");
    
    printf("\n----------DISCONNECTING----------\n\n");

    // Closing connection
    if(llclose(receiver.fileDescriptor, receiver.status) < 0){
        perror("Error: receiver llclose function call\n");
        return ERROR;
    }

    printf("\n----------DISCONNECTED-----------\n\n");
    

    return 0;
}