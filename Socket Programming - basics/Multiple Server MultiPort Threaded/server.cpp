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

void* thread1(void* vargp)
{
    int *var = (int*) vargp;
    int nfsd = *var;
    char buffer[128];
    read(nfsd, buffer, sizeof(buffer));
    
    for(int i = 0; i < strlen(buffer); i++)
    {
        buffer[i] = toupper(buffer[i]);
    }
    send(nfsd, buffer, sizeof(buffer), 0);
    close(nfsd);
    return NULL;
}

void* thread2(void* vargp)
{
    int *var = (int*) vargp;
    int nfsd = *var;
    char buffer[128];
    read(nfsd, buffer, sizeof(buffer));
    
    for(int i = 0; i < strlen(buffer); i++)
    {
        if(!isupper(buffer[i]))
            buffer[i] = toupper(buffer[i]);
        else
            buffer[i] = tolower(buffer[i]);
    }
    send(nfsd, buffer, sizeof(buffer), 0);
    close(nfsd);
    return NULL;
}

void* thread3(void* vargp)
{
    int *var = (int*) vargp;
    int nfsd = *var;
    char buffer[128];
    read(nfsd, buffer, sizeof(buffer));
    
    for(int i = 0; i < strlen(buffer); i++)
    {
        if(i % 2 == 0)
            buffer[i] = toupper(buffer[i]);
        else
            buffer[i] = tolower(buffer[i]);
    }
    send(nfsd, buffer, sizeof(buffer), 0);
    close(nfsd);
    return NULL;
}






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
                    
                    pthread_t threadid;
                    if(i+1 == 1)
                        pthread_create(&threadid, NULL, thread1, (void*) &nsfd);
                    else if(i+1 == 2)
                        pthread_create(&threadid, NULL, thread2, (void*) &nsfd);
                    else
                        pthread_create(&threadid, NULL, thread3, (void*) &nsfd);
                }
            }
        }
    }



    return 0;
}
