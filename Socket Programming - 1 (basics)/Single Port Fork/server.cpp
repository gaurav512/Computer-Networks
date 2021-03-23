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
    sockaddr_in sockaddress;
    sockaddress.sin_family = AF_INET;
    sockaddress.sin_addr.s_addr = INADDR_ANY;
    sockaddress.sin_port = htons(PORT);

    if(bind(sfd, (sockaddr*) &sockaddress, sizeof(sockaddress)) < 0)
    {
        perror("bind FAILED");
        exit(1);
    }

    if(listen(sfd, 5) < 0)
    {
        perror("listen FAILED");
        exit(1);
    }

    while(1)
    {
        sockaddr_in clientAddress;
        int len = sizeof(clientAddress);
        int nsfd;
        if((nsfd = accept(sfd, (sockaddr*)&clientAddress, (socklen_t*)&len)) < 0)
        {
            perror("accept FAILED");
            exit(1);
        }
        int c = fork();
        if(c == 0)
        {
            close(sfd);
            char buff[128];
            read(nsfd, buff, sizeof(buff));
            for(int i = 0; i < strlen(buff); i++)
            {
                buff[i] = toupper(buff[i]);
            }
            send(nsfd, buff, sizeof(buff), 0);
            break;
        }
        else
            close(nsfd);
    }
}