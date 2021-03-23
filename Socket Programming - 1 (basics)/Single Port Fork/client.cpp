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
    sockaddr_in serv_add;
    int len;
    serv_add.sin_family = AF_INET;
    serv_add.sin_port = htons(PORT);
    if(inet_pton(AF_INET, "127.0.0.1", &serv_add.sin_addr) < 0)
    {
        perror("ip address invalid/unsupported");
        exit(1);
    }

    if(connect(sfd, (sockaddr*) &serv_add, sizeof(serv_add)) < 0)
    {
        perror("connect FAILED");
        exit(1);
    }

    char buff[128], buff2[128];
    cin >> buff;
    send(sfd, buff, sizeof(buff), 0);
    read(sfd, buff2, sizeof(buff2));
    cout << "The answer is: " << buff2 << '\n';
    return 0;
}