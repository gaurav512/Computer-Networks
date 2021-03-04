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

using namespace std;

int main()
{
    char buffer[128];
    read(0, buffer, sizeof(buffer));
    
    for(int i = 0; i < strlen(buffer); i++)
    {
        if(!isupper(buffer[i]))
            buffer[i] = toupper(buffer[i]);
        else
            buffer[i] = tolower(buffer[i]);
    }

    send(1, buffer, sizeof(buffer), 0);
    return 0;
}