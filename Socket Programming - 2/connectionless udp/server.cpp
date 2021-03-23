#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<unistd.h>
#include<pthread.h>
#include<semaphore.h>
#include<fcntl.h>
#include<string.h>
#include<ctype.h>
#include<signal.h>
#include<poll.h>

#include<sys/wait.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/shm.h>
#include<sys/msg.h>
#include<sys/uio.h>
#include <sys/socket.h>
#include <sys/errno.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8080

int main()
{
    int sfd;

    if((sfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket FAILED");
        exit(1);
    }

    sockaddr_in serv_addr, client_addr;

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);

    if(bind(sfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("bind FAILED");
        exit(1);
    }

    int len = sizeof(client_addr);

    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    recvfrom(sfd, buffer, sizeof(buffer), MSG_WAITALL, (struct sockaddr *) &client_addr, (socklen_t *) &len);
    buffer[strlen(buffer)] = '\0';
    printf("Client says: %s\n", buffer);

    char msg[] = "Hello client, I am server, the supreme God.";
    sendto(sfd, (char*)msg, strlen(msg), MSG_CONFIRM, (struct sockaddr*)&client_addr, sizeof(client_addr));

    printf("Message to client sent.\n");

    return 0;


}