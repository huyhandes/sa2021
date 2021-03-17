#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

struct sockaddr_in saddr;
struct hostent *h;
int sockfd;
char buffer[1024] = {0}; 
unsigned short port = 8080;
int main(){
    if ((sockfd=socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Error creating socket\n");
        exit(1);
    }
    if ((h=gethostbyname("localhost")) == NULL) {
        printf("Unknown host\n");
        exit(1);
    }
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    memcpy((char *) &saddr.sin_addr.s_addr, h->h_addr_list[0], h->h_length);
    saddr.sin_port = htons(port);
    if (connect(sockfd, (struct sockaddr *) &saddr, sizeof(saddr)) < 0) {
        printf("Cannot connect\n");
        exit(1);
    }
    while (1)
    {   
        char req[1000];
        fgets(req, sizeof(req), stdin);
        send(sockfd , req , strlen(req) , 0); 
        int valread = read( sockfd , buffer, 1024); 
        printf("%s\n",buffer ); 
    }
}