//Funções gerais de escrita (e leitura) para a porta série.
#include "../includes/buffer_utils.h"
//#include "../includes/macrosLD.h"

int writeData(int fd, unsigned char *trama, int size){
  int res = 0, i = 0;
    while (i < size){
      printf("  wr - 0x%x\n", trama[i]);
      res = write(fd, &trama[i], 1);
      if(res < 0){
        perror("Error: writeData function call\n");
        return -1;
      }
      i++;
    }
    return i;
}

void printTramaRead(unsigned char *trama, int size){
    for(int i = 0; i < size; i++)
          printf("  r - 0x%x\n", trama[i]); 
}

void clean_buf(unsigned char *buf, int size){
  memset(buf, '/0', size);
}