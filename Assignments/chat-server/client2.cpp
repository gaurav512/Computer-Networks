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

int c2fd, sfd;

void *readThread(void* vargp)
{
    while(true)
    {
        char buffer[512];
        while(!read(c2fd, buffer, sizeof(buffer)));
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
        temp = "#C2 : " + temp + "~";
        strcpy(buffer2, temp.c_str());
        write(sfd, buffer2, sizeof(buffer2));        
    }
}

int main()
{
    char *c2_fifo = (char*)"/tmp/c2_fifo";
    char *server_fifo = (char*)"/tmp/server_fifo";

    mkfifo(c2_fifo, 0666);
    mkfifo(server_fifo, 0666);

    if((c2fd = open(c2_fifo, O_RDONLY)) < 0)
    {
        perror("Cannot open c2fifo");
    }
    if((sfd = open(server_fifo, O_WRONLY)) < 0)
    {
        perror("Cannot open server fifo");
    }
    
    pthread_t rthread, wthread;
    pthread_create(&rthread, NULL, readThread, NULL);
    pthread_create(&wthread, NULL, writeThread, NULL);

    pthread_join(rthread, NULL);
    pthread_join(wthread, NULL);
    
    return 0;
}