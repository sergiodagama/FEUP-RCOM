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