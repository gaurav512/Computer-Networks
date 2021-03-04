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

using namespace std;

int main()
{
    int sfd;
    if((sfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket FAILED");
        exit(1);
    }
    sockaddr_in serveraddress;
    serveraddress.sin_family = AF_INET;
    serveraddress.sin_port = htons(PORT);
    
    if(inet_pton(AF_INET, "127.0.0.1", &serveraddress.sin_addr) < 0)
    {
        perror("Address not supported/invalid");
        exit(1);
    }
    if(connect(sfd, (struct sockaddr*) &serveraddress, sizeof(serveraddress)) < 0)
    {
        perror("connect FAILED");
        exit(1);
    }
    char msg[128] = "First message!";
    send(sfd, msg, strlen(msg), 0);
    printf("Message sent from client\n");
    return 0;
}