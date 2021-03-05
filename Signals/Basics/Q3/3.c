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

void handler1(int signum)
{
    printf("Received signal from child\n");
    fflush(stdout);
}

void handler2(int signum)
{
    printf("Received signal from parent\n");
    fflush(stdout);
}


int main()
{
    int pid = fork();
    if(pid > 0) // Parent process
    {
        signal(SIGUSR1, handler1);
        sleep(1);
        kill(pid, SIGUSR2);
        sleep(1);
        kill(pid, SIGUSR2);
        sleep(1);
        kill(pid, SIGUSR2);
        sleep(1);
    }
    else  // Child process
    {
        signal(SIGUSR2, handler2);
        sleep(1);
        kill(getppid(), SIGUSR1);
        sleep(1);
        kill(getppid(), SIGUSR1);
        sleep(1);
        kill(getppid(), SIGUSR1);
        sleep(1);
    }
    
    return 0;
}