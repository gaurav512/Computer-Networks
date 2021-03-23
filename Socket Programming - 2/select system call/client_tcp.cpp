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
#define BUFF_LEN 1024
#define MAXCLIENTS 20

using namespace std;

int main()
{
    int sfd;
    if((sfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket at client FAILED");
        exit(1);
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if(connect(sfd, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0)
    {
        perror("connect FAILED");
        exit(1);
    }

    char* msg = (char*)"Hello server! I am TCP Client.";
    write(sfd, msg, strlen(msg));
    printf("Sent message to server\n");


    char buffer[BUFF_LEN];
    memset(buffer, 0, BUFF_LEN);
    read(sfd, buffer, BUFF_LEN);
    printf("Server says: %s\n", buffer);
    close(sfd);

    return 0;
}