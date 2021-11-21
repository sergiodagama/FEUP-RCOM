#include "api.h"

char* byteStuffing(size_t size, unsigned char data[]){
    if(size < 1){
        perror("Error: Size inferior to one");
        //return -1;
    }
    if(data == NULL){
        perror("Error: null array pointers");
        //return -1;
    }
    int index = 1;

    unsigned char* stuffed = malloc(size*2);

    stuffed[0] = FLAG;

    //checking for escapes and flags in data
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
    return stuffed;
}

char* reverseByteStuffing(size_t size, unsigned char stuffed[]){
    if(size < 1){
        perror("Error: Size inferior to one");
        //return -1;
    }
    if(stuffed == NULL){
        perror("Error: null array pointers");
        //return -1;
    }

    int index = 1, i = 1;

    unsigned char* original = malloc(size/2);

    original[0] = FLAG;

    int END_FLAG = 0;

    while(!END_FLAG && index < size*2 - 1){
        if(stuffed[index] == ESCAPE && stuffed[index+1] == FLAG_XORED){
            original[i] = FLAG;
            index+=2;
        }
        else if(stuffed[index] == ESCAPE && stuffed[index+1] == ESCAPE_XORED){
            original[i] = ESCAPE;
            index+=2;
        }
        else if(stuffed[index+1] == FLAG){
            original[i] = stuffed[index];
            original[i+1] = FLAG;
            END_FLAG = 1;
        }
        else{
            original[i] = stuffed[index];
            index++;
        }
        i++;
    }

    return original;
}

//just for testing purposes
int main(){

    unsigned char data[20] = {0x7e, 0x02, 0x03, 0x7e, 0x05, 0x06, 0x7e, 0x08, 0x7d, 0x10, 0x11, 0x12, 0x13, 0x14, 0x7e, 0x7d, 0x17, 0x7e, 0x19, 0x7e};

    size_t size = sizeof data / sizeof data[0];

    printf("DATA BEFORE STUFFING\n");
    printf("Data Size: %u\n", size);

    printf("Trama: ");
    for(int i = 0; i < sizeof(data); i++){
        printf("%x ", data[i]);
    }

    unsigned char* stuffed = byteStuffing(size, data);

    if(stuffed == NULL) perror("Error: error on byteStuffing function");

    printf("\n\nDATA AFTER STUFFING\n");
    printf("Stuffed Size: %u \n", size*2);

    printf("Trama: ");
    for(int i = 0; i < size*2; i++){
        printf("%x ", *(stuffed + i));
    }

    // REVERSE BYTE SUTFFING TESTING
    unsigned char* original = reverseByteStuffing(size*2, stuffed);

    printf("\n\nDATA AFTER REVERSE STUFFING\n");
    printf("Trama: ");
    for(int i = 0; i < size; i++){
        printf("%x ", *(original + i));
    }

    return 0;
}