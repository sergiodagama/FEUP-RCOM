#include "../includes/app_transmitter.h"

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

  int res;

  unsigned char* data;

  while(s > 0){

    if(s < quant) quant = s;

    data = dataChunk(file_info->data, index, quant);

    printf("\t\t\tsize = %ld quant = %ld\n", s, quant);

    s -= quant;
    
    res = llwrite(fd, data, quant);

    if(res == -1){
      printf("Exiting sendDataPacket\n\n");
      return file_info->size - bytes_sent;
    }

    free(data);

    bytes_sent += res;

    index += quant;
  }

  return 0;
}

int sendControlPacket(int fd, enum packet_id id, ControlPacket control_p){
  int bytes_sent = 0;

  
  if(id == START){
    bytes_sent = llwrite(fd, control_p.packet, control_p.size);
  }


  else if(id == END){
  
    control_p.packet[0] =  CP_END;

    bytes_sent = llwrite(fd, control_p.packet,  control_p.size);

  }

  return bytes_sent;
}
