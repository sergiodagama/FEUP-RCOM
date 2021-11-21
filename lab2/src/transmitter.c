#include "../includes/transmitter.h"

int sendPacket(int fd, enum packet_id id, int Ns){
  //llwrite(int fd, char* buffer, int length);

  unsigned char testData[4];

  for(short i = 0; i < 4; i++){
    testData[i] = 'a';
  }
  testData[2] = FLAG;

  if(id == DATA){
    int bytes_sent = llwrite(fd, testData, 4, Ns);

    printf("BYTES SENT: %d\n", bytes_sent);
  }
}

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
      printf("Usage:\t[port_file_name] [file_name_to_send]\tex: /dev/ttyS10 ../pinguim.gif\n");
      return ERROR;
    }
    else if(argc < 3){
      perror("Input error: too few arguments\n");
      printf("Usage:\t[port_file_name] [file_name_to_send]\tex: /dev/ttyS10 ../pinguim.gif\n");
      return ERROR;
    }

    printf("\n######### TRANSMITTER #########\n");

    printf("\n-----------CONNECTING----------\n\n");

    //opening connection
    
    int fd = ERROR; //port file descriptor
    
    //filling application layer info
    ApplicationLayer transmitter;
    transmitter.status = TRANSMITTER;

    //openning connection
    if(llopen(argv[1], transmitter.status, &fd) < 0){
        perror("Error: transmitter llopen function call\n");
        return ERROR;
    }

    //checking errors in file descriptor
    if(fd != ERROR)
        transmitter.fileDescriptor = fd;
    else{
        perror("Error: file descriptor not updated\n");
        return ERROR;
    }
    

    printf("\n------------CONNECTED----------\n\n");

    printf("\n----------SENDING DATA---------\n\n");
    
    sleep(1);

    //sending data TODO
    sendPacket(fd, DATA, 0);

    ////send start packet
    /*sendPacket(START);

    ////loop to send data
    int THERE_IS_DATA = TRUE;

    while(THERE_IS_DATA){
      sendPacket(DATA)
    }

    sendPacket(END);*/

    ////send end packet 

    printf("\n----------ALL DATA SENT----------\n\n");
    
    printf("\n----------DISCONNECTING----------\n\n");

    //closing connection
    if(llclose(transmitter.fileDescriptor, transmitter.status) < 0){
      perror("Error: transmitter llclose function call\n");
      return ERROR;
    }
    

    printf("\n----------DISCONNECTED-----------\n\n");

    return 0;
}