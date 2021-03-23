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
    int sfd1; // TCP socket fd
    cout << "Server started\n";

    struct sockaddr_in server_addr, client_addr;

    // Creating TCP socket
    if((sfd1 = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket FAILED");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if(bind(sfd1, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
    {
        perror("bind FAILED");
        exit(1);
    }
    if(listen(sfd1, MAXCLIENTS) < 0)
    {
        perror("listen FAILED");
        exit(1);
    }
    cout << "TCP socket created\n";


    int sfd2; // UDP socket fd
    if((sfd2 = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror(("socket FAILED"));
        exit(1);
    }
    if(bind(sfd2, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0)
    {
        perror("bind FAILED");
    }
    cout << "UDP socket created\n";

    fd_set rset;
    int max_fd = max(sfd1, sfd2)+1;
    int activity;
    char* msg = (char*)"The server says hi!";
            
    while(1)
    {
        FD_ZERO(&rset);
        // adding fd to the set
        FD_SET(sfd1, &rset);
        FD_SET(sfd2, &rset);

        activity = select(max_fd, &rset, NULL, NULL, NULL);

        if(FD_ISSET(sfd1, &rset)) 
        {
            int len = sizeof(client_addr);
            int nsfd;
            if((nsfd = accept(sfd1, (struct sockaddr*) &client_addr, (socklen_t *) &len)) < 0)
            {
                perror("accept FAILED");
                exit(1);
            }
            int c = fork();
            if(c == 0)
            {
                close(sfd1);
                char buffer[BUFF_LEN];
                memset(buffer, 0, BUFF_LEN);
                read(nsfd, buffer, BUFF_LEN);
                printf("TCP client says: %s\n", buffer);
                write(nsfd, msg, strlen(msg));
                close(nsfd);
                exit(0);
            }
            close(nsfd);
        }

        if(FD_ISSET(sfd2, &rset))
        {
            int len = sizeof(client_addr);
            char buffer[BUFF_LEN];
            memset(buffer, 0, BUFF_LEN);
            recvfrom(sfd2, buffer, BUFF_LEN, 0, (struct sockaddr*) &client_addr, (socklen_t*)&len);
            printf("UDP client says: %s\n", buffer);
            sendto(sfd2, msg, strlen(msg), 0, (struct sockaddr*) &client_addr, sizeof(client_addr));            
        }

    }

    return 0;
}