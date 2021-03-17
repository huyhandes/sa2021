#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <arpa/inet.h>

// typedef struct {
//     unsigned long s_addr; // load with inet_aton()
// }in_addr;

// typedef struct {
//     short sin_family; // e.g. AF_INET
//     unsigned short sin_port; // e.g. htons(3490)
//     in_addr sin_addr; // see struct in_addr, below
//     char sin_zero[8]; // zero this if you want to
// } sockaddr_in;


int sockfd, clientfd;
struct sockaddr_in saddr, caddr;
unsigned int clen;
unsigned short port = 8080;
char buffer[1024] = {0}; 
int main(){
    if ((sockfd=socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Error creating socket\n");
        exit(1);
    }
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);
    saddr.sin_port = htons(port);
    if (bind(sockfd, (struct sockaddr *) &saddr, sizeof(saddr)) < 0) {
        printf("Error binding\n");
        exit(1);
    }
    if (listen(sockfd, 5) < 0) {
        printf("Error listening\n");
        exit(1);
    }
    clen=(sizeof(caddr));
    if ((clientfd=accept(sockfd, (struct sockaddr *) &caddr, (socklen_t*)&clen)) < 0) {
        printf("Error accepting connection\n");
        exit(1);
    }
    while (1)
    {
        int valread = read(clientfd, buffer, 1024); 
        printf("%s\n",buffer ); 
        char res[1000];
        fgets(res, sizeof(res), stdin);
        send(clientfd , res , strlen(res),0); 
    }
}