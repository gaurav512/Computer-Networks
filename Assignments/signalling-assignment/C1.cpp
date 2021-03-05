#include<bits/stdc++.h>
#include<stdio.h>
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
#include<sys/shm.h>
#include<ctype.h>
#include<signal.h>

using namespace std;

int main()
{
    mkfifo("/tmp/C1fifo", 0666);
    int fd = open("/tmp/C1fifo", O_WRONLY);
    char buffer[1000];
    while(1)
    {
        printf("Enter data in C1: ");
        scanf("%s", buffer);
        fflush(stdout);
        write(fd, buffer, sizeof(buffer));
        memset(buffer, 0, sizeof(buffer));
    }
    close(fd);
    unlink("/tmp/C1fifo");
    return 0;
}