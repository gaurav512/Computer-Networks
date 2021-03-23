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

#define PORT1 5555
#define PORT2 8080
#define PORT3 9090
#define N 3

using namespace std;

int main()
{
    int ports[N] = {PORT1, PORT2, PORT3};
    int choice;
    cout << "1. Capitalize\n2. Opposite case\n3. Alternate case\nEnter your choice: ";
    cin >> choice;
    
    int sfd;
    if((sfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket FAILED");
        exit(1);
    }
    sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(ports[choice-1]);
    
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) < 0)
    {
        perror("ip address invalid/unsupported");
        exit(1);
    }

    if(connect(sfd, (sockaddr*) &serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("connect FAILED");
        exit(1);
    }

    char buffer[128], buffer2[128];
    cout << "Enter string: ";
    cin >> buffer;
    send(sfd, buffer, sizeof(buffer), 0);
    read(sfd, buffer2, sizeof(buffer2));
    cout << "Your modified string is: " << buffer2 << '\n';
    return 0;
}