#include "../includes/app_transmitter.h"

/**
 * @brief Transmitter application layer main function
 * 
 * @param argc number of arguments passed to transmitter
 * @param argv the actual arguments -> ./[program_name] [port_file_name] [file_name_to_send] [delay]
 * @return negatice value in case of error, 0 otherwise
 */
int main(int argc, char** argv){

    if(argc > 4){
      perror("Input error: too many arguments\n");
      printf("Usage:\t[port_file_name] [file_name_to_send]\tex: /dev/ttyS10 ../pinguim.gif\n");
      return ERROR;
    }
    else if(argc < 4){
      perror("Input error: too few arguments\n");
      printf("Usage:\t[port_file_name] [file_name_to_send]\tex: /dev/ttyS10 ../pinguim.gif\n");
      return ERROR;
    }

    printf("\n######### TRANSMITTER #########\n");

    printf("\n-----------CONNECTING----------\n\n");

  
    
    int fd = ERROR;

    unsigned int delay = atoi(argv[3]);
    
 
    ApplicationLayer transmitter;
    transmitter.status = TRANSMITTER;


    if(llopen(argv[1], transmitter.status, &fd) < 0){
        perror("Error: transmitter llopen function call\n");
        return ERROR;
    }

  
    if(fd != ERROR)
        transmitter.fileDescriptor = fd;
    else{
        perror("Error: file descriptor not updated\n");
        return ERROR;
    }

    printf("\n------------CONNECTED----------\n\n");

    FILE *file_fd;
    FileInfo file_info;

    char *file_name = malloc(200);
    strcpy(file_name, "files/transfer/");
    strcat(file_name, argv[2]);

    printf("FILE: %s\n", file_name);

    if((file_fd = fopen(file_name, "rb")) == NULL){
      perror("Error opening file to send\n");
      return ERROR;
    }
    
    if(fillInfo(&file_info, file_fd, argv[2])<0){ 
      perror("Error filling file information\n");
      return ERROR;
    }

    printf("\n----------SENDING DATA---------\n\n");
    
    free(file_name);

    sleep(1);

    ControlPacket control_p = createControlPacket(&file_info);

    //printf("%u \n", delay);

    printf("-------Sending control packet------ \n");
    
    sleep(delay);

   
    sendControlPacket(fd, START, control_p);
   
    sendDataPacket(fd, &file_info, delay);

    
    sendControlPacket(fd, END, control_p);

    fclose(file_fd);
    free(control_p.packet);

    printf("\n----------ALL DATA SENT----------\n\n");
    
    printf("\n----------DISCONNECTING----------\n\n");

   
    if(llclose(transmitter.fileDescriptor, transmitter.status) < 0){
      perror("Error: transmitter llclose function call\n");
      return ERROR;
    }
    
    printf("\n----------DISCONNECTED-----------\n\n");

    return 0;
}
