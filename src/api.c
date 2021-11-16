#include "api.h"

char* byteStuffing(size_t size, unsigned char data[], size_t* trimmed_size){

    if(size < 1){
        perror("Error: Size inferior to one");
        //return -1;
    }
    if(data == NULL){
        perror("Error: null array pointers");
        //return -1;
    }
    int index = 1;

    unsigned char stuffedRaw[size*2];

    stuffedRaw[0] = FLAG;

    //checking for escapes and flags in data
    for(int i = 1; i < size-1; i++){
        if(data[i] == FLAG){
            stuffedRaw[index] = ESCAPE;
            stuffedRaw[index+1] = FLAG_XORED;
            index+=2;
        }
        else if(data[i] == ESCAPE){
            stuffedRaw[index] = ESCAPE;
            stuffedRaw[index+1] = ESCAPE_XORED;
            index+=2;
        }
        else{
            stuffedRaw[index]  = data[i];
            index++;
        }
        stuffedRaw[index] = FLAG;
    }

    //trimming the stuffed array 
    unsigned char* stuffedTrimmed = malloc(index+1);

    for(int i=0; i < index+1; i++){
        stuffedTrimmed[i] = stuffedRaw[i];
        (*trimmed_size)++;
    }
    return stuffedTrimmed;
}

//just for testing purposes
int main(){

    unsigned char data[20] = {0x7e, 0x02, 0x03, 0x7e, 0x05, 0x06, 0x7e, 0x08, 0x7d, 0x10, 0x11, 0x12, 0x13, 0x14, 0x7e, 0x7d, 0x17, 0x7e, 0x19, 0x7e};

    size_t size = sizeof data / sizeof data[0];
    
    printf("DATA BEFORE STUFFING\n");
    printf("Data Size: %u\n\n", size);

    printf("Trama: ");
    for(int i = 0; i < sizeof(data); i++){
        printf("%x ", data[i]);
    }
    printf("\n");

    size_t final_size;

    unsigned char* stuffed = byteStuffing(size, data, &final_size);

    if(stuffed == NULL) perror("Error: error on byteStuffing function");

    printf("\nDATA AFTER STUFFING\n");
    printf("Trimed Size: %u \n\n", final_size);

    printf("Trama: ");
    for(int i = 0; i < final_size; i++){
        printf("%x ", *(stuffed + i));
    }
    return 0;
}