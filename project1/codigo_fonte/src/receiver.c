#include "../includes/app_receiver.h"
#include <time.h>

/**
 * @brief Receiver application layer main function
 * 
 * @param argc number of arguments passed to receiver
 * @param argv the actual arguments -> ./[program_name] [port_file_name] [delay] [generate errors 0-100000]
 * @return negatice value in case of error, 0 otherwise 
 */

int main(int argc, char** argv){

    // Checking for user input error
    if(argc < 4){
        perror("Input error: too few arguments\n");
        printf("Usage:\t[port_file_name]\tex: /dev/ttyS11\n");
        return ERROR;
    }

    printf("\n########### RECEIVER ##########\n");

    printf("\n-----------CONNECTING----------\n\n");

    int fd = ERROR; //port file descriptor

    unsigned int delay = atoi(argv[2]);

    unsigned int genErrors = atoi(argv[3]);
    
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

    srand(time(NULL));

    
    file_size = receiveStartPacket(fd, name, delay, genErrors);

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

    FILE *fp_test;
    fp_test = fopen("test_data.txt", "a");

    struct timespec begin, end; 
    clock_gettime(CLOCK_REALTIME, &begin);

    while(NOT_END){

        clean_buf(packet, I_FRAME_SIZE);
        clean_buf(data, DATA_SIZE);

    
        llread(fd, packet, delay, genErrors);
       

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

    clock_gettime(CLOCK_REALTIME, &end);
    long seconds = end.tv_sec - begin.tv_sec;
    long nanoseconds = end.tv_nsec - begin.tv_nsec;
    double elapsed = seconds + nanoseconds*1e-9;

    fprintf(fp_test, "%lu %f\n", file_size, elapsed);

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