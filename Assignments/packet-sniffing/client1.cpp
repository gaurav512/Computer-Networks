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
#include<netinet/tcp.h>

#define PORT 8080
#define BUFFER_LENGTH 1024

using namespace std;

int main()
{
    int sfd;
    int nsfd;

    cout << "Client is running...\n\n";

    sockaddr_in sock_addr, server_sock_addr;

    if ((sfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket FAILED");
        exit(EXIT_FAILURE);
    }

    sock_addr.sin_addr.s_addr = INADDR_ANY;
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = htons(PORT);

    if (connect(sfd, (sockaddr *)&sock_addr, sizeof(sock_addr)) < 0)
    {
        perror("connect FAILED");
        exit(EXIT_FAILURE);
    }

    char write_buffer[BUFFER_LENGTH];
    char read_buffer[BUFFER_LENGTH];

    cout << "Enter a message: \n";
    string msg;
    getline(cin, msg);
    cout << "\nSending message to server...\n";
    strcpy(write_buffer, msg.c_str());
    send(sfd, write_buffer, sizeof(write_buffer), 0);
    cout << "Sent message to server!\n\n";
    
    
    cout << "Waiting for message from server...\n";
    recv(sfd, read_buffer, sizeof(read_buffer), 0);
    cout << "Server says: " << read_buffer << "\n";

    close(sfd);
    return 0;
}