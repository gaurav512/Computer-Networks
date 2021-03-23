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
    if((sfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket at client FAILED");
        exit(1);
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    char* msg = (char*)"Hey server! I am UDP client.";
    sendto(sfd, msg, strlen(msg), 0, (struct sockaddr*) &server_addr, sizeof(server_addr));
    printf("Message sent to server.\n");

    char buffer[BUFF_LEN];
    memset(buffer, 0, BUFF_LEN);
    int len = sizeof(server_addr); 
    recvfrom(sfd, buffer, BUFF_LEN, 0, (struct sockaddr*) &server_addr, (socklen_t *)&len);
    printf("Server says: %s\n", buffer);

    close(sfd);

   return 0;
}