#define ESCAPE 0x7d
#define FLAG 0x7e
#define VALUE_TO_XOR 0x20

#include <stdio.h>
#include <stdlib.h>

int byteStuffing(size_t size, unsigned char data[], unsigned char stuffedData[]){

    int index = 1;

    unsigned char xored1 = VALUE_TO_XOR ^ FLAG;

    unsigned char xored2 = VALUE_TO_XOR ^ ESCAPE;

    printf("XORED1: %x\n", xored1);
    printf("XORED2: %x\n\n", xored2);

    //size_t size = sizeof data / sizeof data[0];

    printf("SIZE: %u\n", size);

    stuffedData[0] = FLAG;

    //go through all the data array
    for(int i = 1; i < size - 1; i++){
        printf("%d\n", i);
        if(data[i] == FLAG){
            printf("flag: %d\n", i);
            stuffedData[index] = ESCAPE;
            stuffedData[index+1] = xored1;
            index +=2;
        }
        else if(data[i] == ESCAPE){
            printf("escape: %d\n", i);
            stuffedData[index]  = ESCAPE;
            stuffedData[index+1]  = xored2;
            index += 2;
        }
        else{
            stuffedData[index]  = data[i];
            index++;
        }
        stuffedData[index] = FLAG;
    }
    return index;  //check for malloc erros later
}


//just for testing purposes
int main(){

    unsigned char data[20] = {0x7e, 0x02, 0x03, 0x7e, 0x05, 0x06, 0x7e, 0x08, 0x7d, 0x10, 0x11, 0x12, 0x13, 0x14, 0x7e, 0x7d, 0x17, 0x7e, 0x19, 0x7e};

    //unsigned char data[20] = {0x7e, 0x02, 0x03, 0x7e, 0x05, 0x06, 0x7e};

    size_t size = sizeof data / sizeof data[0];

    printf("SIZE OUT: %u\n", size);
    
    printf("DATA BEFORE STUFFING\n\n");

    for(int i = 0; i < sizeof(data); i++){
        printf("%x ", data[i]);
    }

    printf("\n\n");

      //because the worst case all the characters have to be stuffed
    unsigned char stuffedData[size*2];

    int sizeOf_buffed = byteStuffing(size, data, stuffedData);
     
    unsigned char trimed[sizeOf_buffed + 1];

    for(int i =0; i < sizeOf_buffed + 1; i++){
        trimed[i] = stuffedData[i];
    }


    printf("DATA AFTER STUFFING\n\n");

    for(int i = 0; i < sizeof(stuffedData); i++){
        printf("%x ", *(stuffedData + i));
    }

    printf("\n");

        printf("DATA AFTER TRIMED\n\n");

    for(int i = 0; i < sizeof(trimed); i++){
        printf("%x ", *(trimed + i));
    }

    return 0;
}