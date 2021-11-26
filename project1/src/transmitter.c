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
  control.packet[i++] = L1;  //L1

  for (int j = L1-1; j>=0; j--) {  //V1
    control.packet = (unsigned char *)realloc(control.packet, (i + 1));
    control.packet[i++] = V1[j];
  }

  // file name
  control.packet = (unsigned char *)realloc(control.packet, (i + 1));
  control.packet[i++] = 0x1; //T2 = nome do ficheiro
  control.packet = (unsigned char *)realloc(control.packet, (i + 1));
  control.packet[i++] = strlen(file_info->name);  //L2
  control.packet = (unsigned char *)realloc(control.packet, (i + 1));

  for (int j = 0; j < strlen(file_info->name); j++) {  //V2
    control.packet[i++] = file_info->name[j];
    control.packet = (unsigned char *)realloc(control.packet, (i + 1));
  }

  control.size = i;
  return control;
}

int sendDataPacket(int fd, FileInfo* file_info){   
  int quant = DATA_SIZE;
  int index = 0;

  int s = file_info->size;
  int bytes_sent = 0; 

  while(s > 0){
    if(s < quant) quant = s;

    s -= quant;

    bytes_sent += llwrite(fd, dataChunk(file_info->data, index, quant), quant);

    index += quant;
  }

  printf("TOTAL BYTES SENT IN DATA PACKETS: %d\n", bytes_sent);

  return 0;
}

int sendControlPacket(int fd, enum packet_id id, ControlPacket control_p){
  int bytes_sent = 0;

  //START PACKET
  if(id == START){
    bytes_sent = llwrite(fd, control_p.packet, control_p.size);

    printf("BYTES SENT IN START CONTROL PACKET: %d\n", bytes_sent);
  }

  //END PACKET
  else if(id == END){
  
    control_p.packet[0] =  CP_END;

    bytes_sent = llwrite(fd, control_p.packet,  control_p.size);

    printf("BYTES SENT IN END CONTROL PACKET: %d\n", bytes_sent);
  }

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
    strcpy(file_name, "files/transfer/");
    strcat(file_name, argv[2]);

    printf("FILE: %s", file_name);

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
    sendControlPacket(fd, START, control_p);

    //loop to send data
    sendDataPacket(fd, &file_info);

    //send end packet 
    sendControlPacket(fd, END, control_p);

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