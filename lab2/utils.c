#include <unistd.h>
#include <signal.h>
#include <stdio.h>

#include "utils.h"
#include "macrosLD.h"

int writeData(int fd, unsigned char *trama, int size){
  int res, i = 0;
    while (i < size){
      printf("wr - 0x%x  - ", trama[i]);
      res = write(fd, &trama[i], 1);
      i++;
    }

    printf("\n");
    return res;
}

int readData(int fd, int *flag, unsigned char *reader, int size){
    int res, i = 0;
    int FINISH = FALSE;

    while (!FINISH) {       /* loop for input */

        //printf("before read\n");

        if ((res = read(fd,&reader[i],1)) < 0){
          if (flag == 1){
            printf("Timed Out\n");
            return -1;
          }
          else{
            perror("Read failed\n");
          }
        }

        printf("r - 0x%x (%d) - ", reader[i], res);

        i++;
        if (i == size) FINISH = TRUE; //mudar a condição de paragem para reconhecer a FLAG
    }

    printf("\n");
    return res;
}

int checkTramaReceived(unsigned char *trama, int size, int typeMsg){
    int is_OK = FALSE;
    int idx = 0;
    unsigned char BCC;

    if (typeMsg == TM_SET){

        BCC = BCC(A_EE, C_SET); //do this in all involving BCCs

        while (idx < size){   

            //Verificações que saem
            if ((idx == 0 || idx == 4) && trama[idx] != FLAG){
                break;
            }
            else if (idx == 1 && trama[idx] != A_EE){
                break;
            }
            else if (idx == 2 && trama[idx] != C_SET){
                break;
            }
            else if (idx == 3 && trama[idx] != BCC){
                break;
            }

            idx++;

            if(idx == size) is_OK = TRUE;
        }
    }
    else if(typeMsg == TM_UA){
        BCC = BCC(A_ER, C_UA);

        while (idx < size){

            if((idx == 0 || idx == 4) && trama[idx] != FLAG){
                break;
            }
            else if (idx == 1 && trama[idx] != A_ER){
                break;
            }
            else if (idx == 2 && trama[idx] != C_UA){
                break;
            }
            else if (idx == 3 && trama[idx] != BCC){
                break;
            }

            idx++;

            if(idx == size) is_OK = TRUE;
        }
    }
    else if (typeMsg == TM_I){
        //Escrever checks para TypeMsg I
    }
    else if (typeMsg == TM_RR){
        //Quando conseguirmos verificar esta
        //como fazer para verificar se o C_RR é o certo?? e o BCC?
        //tem o NS0 e o NS1

        BCC = BCC(A_ER, C_REJ_NS0);

        while (idx < size){

            if((idx == 0 || idx == 4) && trama[idx] != FLAG){
                break;
            }
            else if (idx == 1 && trama[idx] != A_ER){
                break;
            }
            else if (idx == 2 && trama[idx] != C_RR_NS0){
                break;
            }
            else if (idx == 3 && trama[idx] != BCC){
                break;
            }

            idx++;

            if(idx == size) is_OK = TRUE;
        }
    }
    else if (typeMsg == TM_REJ){
        //Escrever checks para TypeMsg REJ
    }
    else if (typeMsg == TM_DISC_EE || typeMsg == TM_DISC_ER){
        //Quando conseguirmos verificar esta

        while (idx < size){

            if((idx == 0 || idx == 4) && trama[idx] != FLAG){
                break;
            }
            else if (idx == 1 && typeMsg == TM_DISC_EE && trama[idx] != A_EE){
                break;
            }
            else if (idx == 1 && typeMsg == TM_DISC_ER && trama[idx] != A_ER){
                break;
            }
            else if (idx == 2 && trama[idx] != C_DISC){
                break;
            }
            else if (idx == 3 && typeMsg == TM_DISC_EE && trama[idx] != BCC(A_EE, C_DISC)){
                break;
            }
            else if (idx == 3 && typeMsg == TM_DISC_ER && trama[idx] != BCC(A_ER, C_DISC)){
                break;
            }

            idx++;

            if(idx == size) is_OK = TRUE;
        }
    }
    else {
        printf("TypeMsg not valid!\n");
    }



  return is_OK;
}