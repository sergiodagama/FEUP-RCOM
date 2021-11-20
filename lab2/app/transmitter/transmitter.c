#include "transmitter.h"

/**
 * @brief Transmitter application layer main function
 * 
 * @param argc number of arguments passed to transmitter
 * @param argv the actual arguments -> ./[program_name] [port_file_name] [file_name_to_send]
 * @return negatice value in case of error, 0 otherwise
 */
int main(int argc, char** argv){

    //checking for user input errors
    if(argc > 3){
      perror("Input error: too many arguments\n");
      printf("Usage:\t[port_file_name] [file_name_to_send]\tex: /dev/ttyS1 test.gif\n");
      return ERROR;
    }
    else if(argc < 3){
      perror("Input error: too few arguments\n");
      printf("Usage:\t[port_file_name] [file_name_to_send]\tex: /dev/ttyS1 test.gif\n");
      return ERROR;
    }

    printf("\n----------TRANSMITTER----------\n\n");

    printf("\n-----------CONNECTING----------\n\n");

    //opening connection
    /*
    if (llopen(argv[1], TRANSMITTER, ) < 0){
      perror("Error: transmitter llopen function call\n");
      return ERROR;
    }
    */

    printf("\n------------CONNECTED----------\n\n");

    printf("\n----------SENDING DATA---------\n\n");
    
    //sending data TODO

    ////send start packet

    ////loop to send data

    ////send end packet 

    printf("\n----------ALL DATA SENT----------\n\n");
    
    printf("\n----------DISCONNECTING----------\n\n");
/*
    //closing connection
    if(llclose(fd, TRANSMITTER) < 0){
      perror("Error: transmitter llclose function call\n");
      return ERROR;
    }
    */

    printf("\n----------DISCONNECTED-----------\n\n");

    return 0;
}