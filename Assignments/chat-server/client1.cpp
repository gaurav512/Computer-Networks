#include<stdio.h>
#include<iostream>
#include<stdlib.h>
#include<sys/types.h>
#include<unistd.h>
#include<sys/wait.h>
#include<pthread.h>
#include<semaphore.h>
#include<fcntl.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<ctype.h>

int c1fd, sfd;

void *readThread(void* vargp)
{
    while(true)
    {
        char buffer[512];
        while(!read(c1fd, buffer, sizeof(buffer)));
        printf("%s\n", buffer);
        fflush(stdout);
    }
}

void *writeThread(void* vargp)
{
    while(true)
    {
        char buffer[512];
        char buffer2[512];
        scanf("%s", buffer);
        std::string temp(buffer);
        temp = "#C1 : " + temp + "~";
        strcpy(buffer2, temp.c_str());
        write(sfd, buffer2, sizeof(buffer2));        
    }
}

int main()
{
    char *c1_fifo = (char*)"/tmp/c1_fifo";
    char *server_fifo = (char*)"/tmp/server_fifo";


    mkfifo(c1_fifo, 0666);
    mkfifo(server_fifo, 0666);

    if((c1fd = open(c1_fifo, O_RDONLY)) < 0)
    {
        perror("Cannot open c1fifo");
        exit(1);
    }
    if((sfd = open(server_fifo, O_WRONLY)) < 0)
    {
        perror("Cannot open server fifo");
        exit(1);
    }
    
    pthread_t rthread, wthread;
    pthread_create(&rthread, NULL, readThread, NULL);
    pthread_create(&wthread, NULL, writeThread, NULL);

    pthread_join(rthread, NULL);
    pthread_join(wthread, NULL);
    
    return 0;
}