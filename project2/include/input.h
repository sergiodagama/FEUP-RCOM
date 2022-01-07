#ifndef INPUT_H_
#define INPUT_H_

#include <string.h>   
#include <stdlib.h> 
#include <stdio.h> 

typedef struct{
    char url[5120];

    char user[1024];
    char password[1024];
    char host[1024];
    char url_path[1024];
  
}url_data;

int inputCheck(char *ur, url_data *data);

int url_converter(char *url, url_data *data);

#endif  