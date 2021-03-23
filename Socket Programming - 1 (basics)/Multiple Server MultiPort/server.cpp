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
    int sfd[N];
    sockaddr_in sockaddress[N];
    int ports[N] = {PORT1, PORT2, PORT3};
    for(int i = 0; i < N; i++)
    {
        if((sfd[i] = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        {
            perror("socket FAILED");
            exit(1);
        }
        sockaddress[i].sin_family = AF_INET;
        sockaddress[i].sin_addr.s_addr = INADDR_ANY;
        sockaddress[i].sin_port = htons(ports[i]);
        
        if(bind(sfd[i], (sockaddr*) &sockaddress[i], sizeof(sockaddress[i])) < 0)
        {
            perror("bind FAILED");
            exit(1);
        }

        if(listen(sfd[i], 5) < 0)
        {
            perror("listen FAILED");
            exit(1);
        }
    }

    int nfds = N;
    struct pollfd pfds[nfds];
    for(int i = 0; i < N; i++)
    {
        pfds[i].fd = sfd[i];
        pfds[i].events = POLLIN;
    }

    char s1[] = "./s1";
    char *args1[] = {s1, NULL}; 
    char s2[] = "./s2";
    char *args2[] = {s2, NULL};
    char s3[] = "./s3";
    char *args3[] = {s3, NULL};

    while(1)
    {
        int ready = poll(pfds, nfds, -1);
        if(ready > 0)
        {
            for(int i = 0; i < N; i++)
            {
                if(pfds[i].revents & POLLIN)
                {
                    sockaddr_in clientAddress;
                    int len = sizeof(clientAddress);
                    int nsfd;
                    if((nsfd = accept(sfd[i], (sockaddr*) &clientAddress, (socklen_t*) &len)) < 0)
                    {
                        perror("accept FAILED");
                        exit(1);
                    }
                    // cout << sfd[i] << ' ' << nsfd << '\n';
                    int p = fork();
                    if(p == 0)
                    {
                        close(sfd[i]);
                        dup2(nsfd, 0);
                        dup2(nsfd, 1);
                        if(i+1 == 1)
                            execv(args1[0], args1);      
                        else if(i+1 == 2)
                            execv(args2[0], args2);
                        else
                            execv(args3[0], args3);
                    }
                    else
                        close(nsfd);
                }
            }
        }
    }



    return 0;
}
