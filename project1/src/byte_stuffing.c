#include "../includes/byte_stuffing.h"

int byteStuffing(size_t size, unsigned char data[], unsigned char* stuffed){
    if(size < 1){
        perror("Error: Size inferior to one");
        //return -1;
    }
    if(data == NULL){
        perror("Error: null array pointers");
        //return -1;
    }
    int index = 1;

   

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
    return 1;
}

int reverseByteStuffing(size_t size, unsigned char stuffed[],  unsigned char* original){
    printf("I'm on deStuff\n");


    if(size < 1){
        perror("Error: Size inferior to one");
        //return -1;
    }
    if(stuffed == NULL){
        perror("Error: null array pointers");
        //return -1;
    }

    int index = 1, i = 1;

    printf("RA\n");

    clean_buf(original, size);

    //unsigned char* original = malloc(size);

    printf("RB\n");

    original[0] = FLAG;

    int END_FLAG = 0;

    while(!END_FLAG){
        if(i == 450){
            break;
        }
        

        if(stuffed[index] == ESCAPE && stuffed[index+1] == FLAG_XORED){
            original[i] = FLAG;
            index+=2;

            printf("\tError here? 1 \n");
            //printf("Inside 1\n");
        }
        else if(stuffed[index] == ESCAPE && stuffed[index+1] == ESCAPE_XORED){
            original[i] = ESCAPE;
            index+=2;
            //printf("Inside 2\n");

            printf("\tError here? 2 \n");
        }
        else if(stuffed[index+1] == FLAG){
        printf("\tEndLOOP - Error here? 3 \n");

            original[i] = stuffed[index];
            original[i+1] = FLAG;
            END_FLAG = 1;
            break;
            //printf("Inside 3\n");
        }
        else{
            original[i] = stuffed[index];
            index++;

            //printf("\t (%d) Error here? 4 \n", index);
            //printf("Inside 4\n");
        }
        i++;
    }

    /*for(int i = 0; i < size;i++){
    printf("VALUE index %d -> %x\n", i, original[i]);
    }*/

    printf("DeStuff ok!\n");
    //return original;
    return 1;
}
