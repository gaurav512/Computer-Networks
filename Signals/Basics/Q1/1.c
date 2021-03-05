// Does not terminate the process the first time we do ctrl+c

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
#include<ctype.h>
#include<signal.h>


void handler(int signum)
{
    printf("I won't terminate\n");
    signal(SIGINT, SIG_DFL);
}

int main()
{
    signal(SIGINT, handler);
    while(1);
    return 0;
}