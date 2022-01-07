#include "../include/download.h"


int main(int argc, char *argv[]){
    if (argc > 2){
        printf("Too many arguments.\n");
        return -1;
    }

    url_data *data = (url_data*)malloc(sizeof(url_data));

    if (inputCheck(argv[1], data) < 0){
        printf("Invalid url\n");
        return -1;
    }
    
    struct hostent *h;
    char * response;

    if (( h = gethostbyname(data->host)) == NULL){
        printf("Error getting IP\n");
        return -1;
    }

    char *ip_addr = inet_ntoa(*((struct in_addr *)h->h_addr));


    /*connect to server ------------------------------------------------*/

    printf("Task: connecting to socket\n");
    int sockRequest = connect_socket(ip_addr, SERVER_PORT);


    response = read_reply(sockRequest);
    if((response[0]=='4') || response[0]=='5'){
        close(sockRequest);
        return -1;
    }


    /*login ------------------------------------------------*/

    if (give_credentials(data, sockRequest) < 0){
        printf("Error sending credentials \n");
        return -1;
    }

    /*enter passive mode ------------------------------------------------*/



    int port = activate_passive_mode(sockRequest);

    if(port == -1){
        printf("Failed to activate passive mode\n");
        close(sockRequest);
        return -1;
    }


    /*connect to the server ------------------------------------------------*/

    int sockReceive = connect_socket(ip_addr, port);



    /*save file ------------------------------------------------ */

    write_cmd(sockRequest, "retr ", data->url_path);
	response = read_reply(sockRequest);
    if((response[0]=='4') || response[0]=='5'){
        close(sockRequest);
        return -1;
    }

	download_file(sockReceive, data->url_path);
	response = read_reply(sockRequest);
    if((response[0]=='4') || response[0]=='5'){
        close(sockRequest);
        return -1;
    }

    /*exit ------------------------------------------------*/

	write_cmd(sockRequest, "quit", "");
	response = read_reply(sockRequest);
    if((response[0]=='4') || response[0]=='5'){
        close(sockRequest);
        return -1;
    }


    if (close(sockRequest)<0) {
        perror("close()");
        exit(-1);
    }
    if (close(sockReceive)<0) {
        perror("close()");
        exit(-1);
    }

    free(data);
    free(response);
    return 0;
}

    