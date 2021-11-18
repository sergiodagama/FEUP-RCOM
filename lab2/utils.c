//Funções gerais de escrita (e leitura) para a porta série.

int writeData(int fd, unsigned char *trama, int size){
  int res, i = 0;
    while (i < size){
      printf("  wr - 0x%x\n", trama[i]);
      res = write(fd, &trama[i], 1);
      i++;
    }

    return res;
}

void printTramaRead(unsigned char *trama, int size){
    for(int i = 0; i < SU_TRAMA_SIZE; i++)
          printf("  r - 0x%x\n", trama[i]); 
}

void clean_buf(unsigned char *buf, int size){
  memset(buf, '/0', size);
}