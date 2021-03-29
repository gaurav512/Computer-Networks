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
#include<sys/socket.h>
#include<sys/errno.h>
#include<sys/un.h>

#include<netinet/if_ether.h>
#include<net/ethernet.h>
#include<netinet/ether.h>
#include<netinet/udp.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<netinet/ip.h>

#define PORT 8080
#define BUFF_LEN 1024
#define MAXCLIENTS 5

using namespace std;

int main()
{
    int sfd;

    if((sfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket FAILED");
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
    
    char buffer[BUFF_LEN];
    memset(buffer, 0, BUFF_LEN);
    while(!read(sfd, buffer, BUFF_LEN));

    cout << buffer;
    fflush(stdout);

    char name[] = "C5";
    while(1)
    {
        char msg[BUFF_LEN];
        memset(msg, 0, BUFF_LEN);
        while(!read(sfd, msg, BUFF_LEN));
        printf("%s", msg);
        fflush(stdout);

        send(sfd, name, sizeof(name), 0);


    }

    while(1);

    return 0;
}