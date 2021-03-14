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


int c1fd, c2fd, c3fd, rfd;


void sendToC1(std::string &s)
{
    char *msg = &s[0];    
    write(c1fd, msg, s.size()+1);
}

void sendToC2(std::string &s)
{
    char *msg = &s[0];      
    write(c2fd, msg, s.size()+1);
}

void sendToC3(std::string &s)
{
    char *msg = &s[0];    
    write(c3fd, msg, s.size()+1);
}


int main()
{

    char *c1_fifo = (char*)"/tmp/c1_fifo";
    char *c2_fifo = (char*)"/tmp/c2_fifo";
    char *c3_fifo = (char*)"/tmp/c3_fifo";
    char *server_fifo = (char*)"/tmp/server_fifo";


    if(mkfifo(c1_fifo, 0666) == -1)
    {
        perror("error");
        exit(1);
    }
    if(mkfifo(c2_fifo, 0666) == -1)
    {
        perror("error");
        exit(1);
    }
    if(mkfifo(c3_fifo, 0666) == -1)
    {
        perror("error");
        exit(1);
    }
    if(mkfifo(server_fifo, 0666) == -1)
    {
        perror("error");
        exit(1);
    } 
    

    if((c1fd = open(c1_fifo, O_WRONLY)) < 0)
    {
        perror("Cannot open C1");
        exit(1);
    }
    if((c2fd = open(c2_fifo, O_WRONLY)) < 0)
    {
        perror("Cannot open C2");
        exit(1);
    }
    if((c3fd = open(c3_fifo, O_WRONLY)) < 0)
    {
        perror("Cannot open C3");
        exit(1);
    }
    if((rfd = open(server_fifo, O_RDONLY)) < 0)
    {
        perror("Cannot open server fifo");
        exit(1);
    }


    while(1)
    {
        char buffer[2048];
        while(!read(rfd, buffer, sizeof(buffer)));
        fflush(stdout);
        for(int i = 0; i < strlen(buffer); i++)
        {
            if(buffer[i] == '#')
            {
                std::string temp = "";
                i++;
                while(buffer[i] != '~')
                {
                    temp += buffer[i++];
                }
                if(temp[1] == '1')
                {
                    sendToC2(temp);
                    sendToC3(temp);
                }
                else if(temp[1] == '2')
                {
                    sendToC1(temp);
                    sendToC3(temp);
                }
                else
                {
                    sendToC1(temp);
                    sendToC2(temp);
                }
            }
        }
    }
    close(c1fd);
    close(c2fd);
    close(c3fd);
    close(rfd);
    
    return 0;
}