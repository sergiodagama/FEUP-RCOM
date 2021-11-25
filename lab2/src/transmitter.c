#include "../includes/transmitter.h"

ControlPacket createControlPacket(FileInfo* file_info){
  ControlPacket control;

  unsigned char V1[sizeof(unsigned long)];
  unsigned long size = file_info->size;
  unsigned int L1 = 0;


  

  for (int c = 0; c < sizeof(unsigned long); c++) {
    V1[c] = (size >> (8 * c)) & 0xff;
    L1 += 1;
  }
  for (int c = sizeof(unsigned long) - 1; c != 0; c--) {
    if (V1[c] != 0) break;
    L1--;
  }


  int i = 0;
  //C
  control.packet = (unsigned char *)malloc(i + 1);
  control.packet[i++] = CP_START;
  control.packet = (unsigned char *)realloc(control.packet, (i + 1));
  // file size
  control.packet[i++] = 0x00; //T1 = tamanho do ficheiro
  control.packet = (unsigned char *)realloc(control.packet, (i + 1));
  control.packet[i++] = L1;

  for (int j = L1-1; j >=0; j--) {
    control.packet = (unsigned char *)realloc(control.packet, (i + 1));
    control.packet[i++] = V1[j];
  }
  control.packet = (unsigned char *)realloc(control.packet, (i + 1));
  // file name
  control.packet[i++] = 0x1; //T1 = nome do ficheiro
  control.packet = (unsigned char *)realloc(control.packet, (i + 1));
  control.packet[i++] = strlen(file_info->name);
  control.packet = (unsigned char *)realloc(control.packet, (i + 1));
  for (int j = 0; j < strlen(file_info->name); j++) {
    control.packet[i++] = file_info->name[j];
    control.packet = (unsigned char *)realloc(control.packet, (i + 1));
  }

  control.size = i;
  return control;



//   unsigned char * converted = malloc(50);

//   ControlPacket *controlpacket = malloc(sizeof(ControlPacket));


//   converted[0] =  CP_START;

//   converted[1] = 0;
//   int i = 0;

//   printf("\n");

//   while(*(file_info->name + i) != '\0'){
//     converted[i+3] = *(file_info->name + i);
//     printf(" (%c) - 0x%x - ", *(file_info->name + i), *(file_info->name + i));
//     i++;
//   }
//   converted[2] = i;

//   printf("\n");

// /*
//   unsigned char size_dataSize[sizeof(unsigned long)];

//   memcpy()

//   size_t size_of_size = sizeof(unsigned long);
// */
//   converted[i+3]= 1;
//   //converted[i+2]= (unsigned short) size_of_size;
//   converted[i+3+1]= 4;
//   converted[i+3+2]= 0;
//   converted[i+3+3]= 0;
//   converted[i+3+4]= 0;
//   converted[i+3+5]= 1;
//   /*
//   for(int k = 0; k < size_of_size; k++){
//     converted[k+i+3] = *(file_info->size + k);
//   }
// */

//   controlpacket->packet = converted;
//   controlpacket->size = i+3+6;

//   return controlpacket;
}

//sendPacket

int sendDataPacket(int fd, FileInfo* file_info){   
  
    int quant = 1024;
    int index = 0;

    int s = file_info->size;
    int bytes_sent = 0; 
    
    while(s > 0){
      if(s < quant) quant = s;

        s -= quant;

      bytes_sent += llwrite(fd, dataChunk(file_info->data, index, quant), quant);

      index += quant;
    }

    printf("BYTES SENT - DATA PACKETS: %d\n", bytes_sent);

/*
    printf("I'm done sending data!\n");
    llwrite(fd, dataChunk(file_info->data, 0, 50), 50);
    */

  return 0;
}

int sendControlPacket(int fd, enum packet_id id, ControlPacket control_p){
  /*
  //TESTING PURPOSES
  unsigned char testData[4];

  for(short i = 0; i < 4; i++){
    testData[i] = 'a';
  }
  testData[2] = FLAG;
*/

  int bytes_sent;

  //START PACKET
  if(id == START){
    bytes_sent = llwrite(fd, control_p.packet, control_p.size);
  }

  //END PACKET
  else if(id == END){
  
    control_p.packet[0] =  CP_END;
    bytes_sent = llwrite(fd, control_p.packet,  control_p.size);
  }

  printf("BYTES SENT in send DATA: %d\n", bytes_sent);
  return bytes_sent;
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

    //checking errors in port file descriptor
    if(fd != ERROR)
        transmitter.fileDescriptor = fd;
    else{
        perror("Error: file descriptor not updated\n");
        return ERROR;
    }

    printf("\n------------CONNECTED----------\n\n");

    //openning file and filling file information
    FILE *file_fd;
    FileInfo file_info;

    char file_name[1000];
    strcpy(file_name, "src/");
    strcat(file_name, argv[2]);


    if((file_fd = fopen(file_name, "rb")) == NULL){
      perror("Error opening file to send\n");
      return ERROR;
    }
    
    if(fillInfo(&file_info, file_fd, argv[2])<0){ 
      perror("Error filling file information\n");
      return ERROR;
    }

    printf("\n----------SENDING DATA---------\n\n");
    
    sleep(1);

    ControlPacket control_p = createControlPacket(&file_info);

    printf("-------Sending control packet------ \n");
    
    printf("size: %d\n\n", control_p.size);

    for(int c = 0; c < control_p.size; c++){
      printf("%x \n", control_p.packet[c]);
    }

    //send start packet
    //sendControlPacket(fd, START, control_p);

    //loop to send data
    sendDataPacket(fd, &file_info);

    //send end packet 
    //sendControlPacket(fd, END, control_p);

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