#include "../includes/byte_stuffing.h"

int byteStuffing(size_t size, unsigned char data[], unsigned char* stuffed){
    if(size < 1){
        perror("Error: Size inferior to one");
        return -1;
    }
    if(data == NULL){
        perror("Error: null array pointers");
        return -1;
    }
    int index = 1;

   

    stuffed[0] = FLAG;

    for(int i = 1; i < size-1; i++){
        if(data[i] == FLAG){
            stuffed[index] = ESCAPE;
            stuffed[index+1] = FLAG_XORED;
            index+=2;
        }
        else if(data[i] == ESCAPE){
            stuffed[index] = ESCAPE;
            stuffed[index+1] = ESCAPE_XORED;
            index+=2;
        }
        else{
            stuffed[index]  = data[i];
            index++;
        }
        stuffed[index] = FLAG;

        for(int i = index +1; i < size * 2; i++){
            stuffed[i] = 0;
        }
    }
    return 1;
}

int reverseByteStuffing(int *size, unsigned char stuffed[],  unsigned char* original){

    if(*size < 1){
        perror("Error: Size inferior to one");
        return -1;
    }
    if(stuffed == NULL){
        perror("Error: null array pointers");
        return -1;
    }

    int index = 1, i = 1;

    original[0] = FLAG;

    int END_FLAG = 0;

    while(!END_FLAG){
        if(i == 450){
            break;
        }
        

        if(stuffed[index] == ESCAPE && stuffed[index+1] == FLAG_XORED){
            original[i] = FLAG;
            index+=2;
        }
        else if(stuffed[index] == ESCAPE && stuffed[index+1] == ESCAPE_XORED){
            original[i] = ESCAPE;
            index+=2;
        }
        else if(stuffed[index] == FLAG){
            original[i] = stuffed[index];
            END_FLAG = 1;
        }
        else{
            original[i] = stuffed[index];
            index++;
        }
        i++;
    }

    *size=i;
    return 1;
}
