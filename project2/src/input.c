#include "../include/input.h"

int inputCheck(char *url, url_data *data){
    if(strncmp("ftp://", url, 6) != 0){
        printf("Url header wrong \n");
        return -1;
    }

    
    return url_converter(url, data);
}

int url_converter(char *url, url_data *data){//não suporta user anonimo
    int state = 0;
    int counter = 0;
    long urlSize = strlen(url);

    if(strchr(url, '@') == NULL){
        state = 2;
        strcpy(data->user, "anonymous");
        strcpy(data->password, "qualquer-password");
    }

    for(int c = 6; c < urlSize; c++){
        char url_char = url[c];

        switch (state){
            case 0:
                if(url_char == ':'){
                    state = 1;
                    counter = 0;
                }
                else{
                    data->user[counter] = url_char;
                    counter++;
                }
                break;
            
            case 1:
                if(url_char == '@'){
                    state = 2;
                    counter = 0;
                }
                else{
                    data->password[counter] = url_char;
                    counter++;
                }
                break;

            case 2:
                if(url_char == '/'){
                    state = 3;
                    counter = 0;
                }
                else{
                    data->host[counter] = url_char;
                    counter++;
                }
                break;

            case 3:
                data->url_path[counter] = url_char;
                counter++;
                break;


        }

    }

    if(state != 3){
        printf("%d \n\n", state);
        printf("Wrong url structure\n");
        return -1;
    }

    strcpy(data->url, url);

    return 0;
}