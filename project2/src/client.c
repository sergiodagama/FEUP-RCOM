#include "../include/client.h"


void write_cmd(int sockfd, char *cmd, char *arg){
	write(sockfd, cmd, strlen(cmd));
	write(sockfd, arg, strlen(arg));
	write(sockfd, "\n", 1);
	printf("Wrote: %s%s\n", cmd, arg);
}

char * read_reply(int sockfd){
	char code;
	char *r = malloc(1024);
	
	size_t n = 0;
	ssize_t read;

	FILE* fp = fdopen(sockfd, "r");
	while((read = getline(&r, &n, fp)) != -1) {
		if(r[3] == ' ') break;
	}

	r[1023] = '\0';
	printf("Reply: %s\n", r);

	return r;
}

int give_credentials(url_data *data, int sockfd){
    char r;
    write_cmd(sockfd, "user ", data->user);
	r = read_reply(sockfd);
	if(r == '4' || r == '5'){
		close(sockfd);
		return -1;
	}

	write_cmd(sockfd, "pass ", data->password);
	r = read_reply(sockfd);
	if(r == '4' || r == '5'){
		close(sockfd);
		return -1;
	}
    return 0;
}

int check_reply(char r){
    if((r=='4') || r=='5'){
        return -1;
    }
    return 0;
}

int connect_socket(char * ip_addr, int port){
    int	sockfd;
	struct sockaddr_in server_addr;
	
	/*server address handling*/
	bzero((char*)&server_addr,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(ip_addr);
	server_addr.sin_port = htons(port);
    
	/*open an TCP socket*/
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		fprintf(stderr, "Error opening socket!\n");
        return -1;
	}

	/*connect to the server*/
	if(connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		fprintf(stderr, "Error connecting to server!\n");
        return -1;
	}
    
    return sockfd;
}

int activate_passive_mode(int sockfd){
    char first_byte[4];
	char second_byte[4];
	
	write_cmd(sockfd, "pasv", "");
	
	char *res = malloc(1024);
	res = read_reply(sockfd);
	if(res[0] == '3' || res[0] == '4' || res[0] == '5') {
        free(res);
        return -1;
    }


    strtok(res, "(");
    char * args = strtok(NULL, ")");

    printf("args: %s\n", args);

	int ip[4], port[2];
	sscanf(args, "%d,%d,%d,%d,%d,%d", &ip[0], &ip[1], &ip[2], &ip[3], &port[0], &port[1]);

    printf("ip: %d,%d,%d,%d\n", ip[0], ip[1], ip[2], ip[3]);
    printf("port: %d,%d\n", port[0], port[1]);

    free(res);

    return port[0] * 256 + port[1];

    // int c = 0;
    // while (c<strlen(res)){
    //     if(res[c] == '('){
    //         c++;
    //         break;
    //     }
    //     c++;
    // }

    // char * resValues = malloc(strlen(res)-c-1);
    // for(int s = c; s < strlen(res)-1; s++){
    //     resValues[s] =res[s];
    // }
        

	// strtok(resValues, ",");
	// for(int i=0; i<4; i++) strcpy(first_byte, strtok(NULL, ","));
	// strcpy(second_byte, strtok(NULL, ","));

	// free(res);
    // free(resValues);
	// return atoi(first_byte)*256 + atoi(second_byte);
}

int download_file(int sockfd, char * url_path){
    char* filename = basename(url_path);

	FILE *f = fopen(filename, "wb+");
	
	if(f == NULL){
		perror("fopen()");
		exit(1);
	}

	char buffer[1024];
	int bytes_read;
	while((bytes_read = read(sockfd, buffer, 1024)) > 0){
		fwrite(buffer, bytes_read, 1, f);
	}

	fclose(f);

    return 0;
}