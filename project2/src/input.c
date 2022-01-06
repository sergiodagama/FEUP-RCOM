#include "../include/input.h"

int inputCheck(char *url, url_data *data){
    if(strncmp("ftp://", url, 6) != 0){
        return -1;
    }

    


}