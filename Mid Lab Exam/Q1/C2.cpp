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

#define PORT 8081
#define BUFF_LEN 1024
#define MAXCLIENTS 5

using namespace std;

#define UNIXSTR_PATH "/tmp/fd-pass.socket"

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

    // Get S2's port number
    char getport[BUFF_LEN];
    memset(getport, 0, BUFF_LEN);
    read(sfd, getport, BUFF_LEN);

    const int PORT2 = atoi(getport);

    // cout << "*" << PORT2 << "*";

    int s2fd;
    if((s2fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket FAILED");
        exit(1);
    }

    sockaddr_in serv_addr, client_addr;

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT2);
    
    while(1)
    {
        char msg[BUFF_LEN];
        memset(msg, 0, BUFF_LEN);
        cout << "Enter message: ";
        fflush(stdout);
        while(!read(STDIN_FILENO, msg, BUFF_LEN));
     
        // cout << msg ;
        sendto(s2fd, msg, BUFF_LEN, MSG_CONFIRM, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

        printf("Message to server sent.\n");
        fflush(stdout);
        
    }

    return 0;
}