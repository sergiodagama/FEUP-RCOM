#include "../include/download.h"

/**
 * The struct hostent (host entry) with its terms documented

    struct hostent {
        char *h_name;    // Official name of the host.
        char **h_aliases;    // A NULL-terminated array of alternate names for the host.
        int h_addrtype;    // The type of address being returned; usually AF_INET.
        int h_length;    // The length of the address in bytes.
        char **h_addr_list;    // A zero-terminated array of network addresses for the host.
        // Host addresses are in Network Byte Order.
    };

    #define h_addr h_addr_list[0]	The first address in h_addr_list.
*/
struct hostent* get_ip(char *hostname){
    // struct hostent *h = (struct hostent *) malloc(sizeof(struct hostent));
    struct hostent *h;
    if ((h = gethostbyname(hostname) == NULL)) {
        herror("gethostbyname()");
        exit(-1);
    }

    // printf("Host name  : %s\n", h->h_name);
    // printf("IP Address : %s\n", inet_ntoa(*((struct in_addr *) h->h_addr)));

    return h;
}




/*./download ftp://[<user>:<password>@]<host>/<url-path>*/
int main(int argc, char *argv[]){
    printf("%s \n", argv[1]);
    if (argc > 2)
        printf("**** No arguments needed. They will be ignored. Carrying ON.\n");

    url_data *data = (url_data*)malloc(sizeof(url_data));

    if (inputCheck(argv[1], data) < 0){
        printf("Invalid url\n");
        return -1;
    }

    printf("Finished input check\n");

    printf("host: %s \n", data->host);
    
    struct hostent *h;
    if (( h = gethostbyname(data->host)) == NULL){
        printf("Error getting IP\n");
        return -1;
    }

    printf("Got the ip\n");

    int sockReq;

    /*connect to server ------------------------------------------------*/

    char *ip_addr = inet_ntoa(*((struct in_addr *)h->h_addr));

    printf("here\n");

    if(ip_addr == NULL){
        printf("IP null\n");
        return -1;
    }
    

    printf("IP: %s\n", ip_addr);


    sockReq = connect_socket(ip_addr, SERVER_PORT);

    printf("Finished connecting to socket\n");

    /*read connect response*/

    char * response;
    response = read_reply(sockReq);
    if((response[0]=='4') || response[0]=='5'){
        close(sockReq);
        return -1;
    }

    printf("Finished connecting to server 1\n");


    /*login ------------------------------------------------*/

    if (give_credentials(data, sockReq) < 0){
        printf("Error sending credentials \n");
        return -1;
    }

    printf("Finished login\n");

    /*enter passive mode ------------------------------------------------*/



    int port = activate_passive_mode(sockReq);

    if(port == -1){
        printf("Failed to activate passive mode\n");
        return -1;
    }

    printf("Finished activating passive mode\n");

    /*connect to the server ------------------------------------------------*/

    int sockReceive = connect_socket(ip_addr, port);

    printf("Finished connecting to server 2\n");


    /*save file ------------------------------------------------ */
    write_cmd(sockReq, "retr ", data->url_path);
	response = read_reply(sockReq);
    if((response[0]=='4') || response[0]=='5'){
        close(sockReq);
        return -1;
    }

    printf("Sended retr cmd\n");

	download_file(sockReceive, data->url_path);
	response = read_reply(sockReq);
    if((response[0]=='4') || response[0]=='5'){
        close(sockReq);
        return -1;
    }

    printf("Finished downloading the file\n");

	// Quit

	write_cmd(sockReq, "quit", "");
	response = read_reply(sockReq);
    if((response[0]=='4') || response[0]=='5'){
        close(sockReq);
        return -1;
    }

    printf("Sended quit cmd\n");

    /*exit ------------------------------------------------*/


    if (close(sockReq)<0) {
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

    