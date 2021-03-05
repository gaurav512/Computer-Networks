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

void handler(int signum, siginfo_t* info, void* vp) 
{
    printf("Signal received in Process %d. This signal was received from Process %d\n", getpid(), info->si_pid);
}

int main()
{
    int p = fork();
    
    if(p < 0)
    {
        perror("Error: ");
        exit(1);
    }

    if(p > 0)   // Parent
    {
        sleep(1);
        kill(p, SIGUSR1);
        printf("Parent pid is %d\n", getpid());

    }
    else // Child process
    {
        // sleep(1);
        struct sigaction newaction;
        newaction.sa_flags = SA_SIGINFO;
        newaction.sa_sigaction = handler;
        sigaction(SIGUSR1, &newaction, NULL);
        printf("Child pid is %d\n", getpid());
        while(1);
    }
    
    return 0;
}