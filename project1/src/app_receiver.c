#include "../includes/app_receiver.h"


int checkControlPacket(enum packet_id id, unsigned char* frame){

    if(id == START){
        if(frame[4] == CP_START && frame[5] == 0){
            return TRUE;
        }
         
        else{ 
            return FALSE;
        }
    }
    if(id == END){
        if(frame[4] == CP_END && frame[5] == 0) return TRUE;
        else return FALSE;
    }
}

unsigned long receiveStartPacket(int fd, unsigned char* name){
    unsigned char* packet = malloc(I_FRAME_SIZE);

    unsigned long file_size = 0; 

    while(TRUE){
        llread(fd, packet);
    

        if(checkControlPacket(START, packet)){
            break;
        } 
    }

    int L1_idx = 6;
    size_t size_of_sf = packet[L1_idx];

    for (int i = 1; i <= size_of_sf; i++){
        file_size += (packet[L1_idx+i]) << 8 * (size_of_sf-i);
    }


    int L2_idx = L1_idx + size_of_sf + 2;
    size_t size_of_fname = packet[L2_idx];

    for (int j = 1; j <= size_of_fname; j++){
        name[j-1] = packet[L2_idx+j];
    }


    return file_size; 
}