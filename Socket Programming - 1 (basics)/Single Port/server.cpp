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
    sockaddr_in client_sockaddress;

    sockaddress.sin_family = AF_INET;
    sockaddress.sin_addr.s_addr = INADDR_ANY;
    sockaddress.sin_port = htons(PORT);

    if(bind(sfd, (struct sockaddr *) &sockaddress, sizeof(sockaddress)) < 0)
    {
        perror("bind FAILED");
        exit(1);
    }
    if(listen(sfd, 5) < 0)
    {
        perror("listen FAILED");
        exit(1);
    }
    int nsfd, addrlen = sizeof(sockaddress);
    if((nsfd = accept(sfd, (struct sockaddr *) &sockaddress, (socklen_t*) &addrlen)) < 0)
    {
        perror("accept FAILED");
        exit(1);
    }
    char buffer[128];
    read(nsfd, buffer, 128);
    cout << "Message received at server: " << buffer << '\n';
    return 0;
}