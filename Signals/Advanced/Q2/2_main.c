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

void handler(int signum)
{
    printf("Received a signal\n");
    exit(0);
}

int main()
{
    key_t key;
    key = ftok("shm", 64);
    int shmid = shmget(key, sizeof(int), IPC_CREAT | 0666);
    int *shmval = (int *) shmat(shmid, NULL, 0);
    *shmval = getpid();
    signal(SIGUSR1, handler);

    int pid = fork();
    if(pid > 0)
    {
        if(fork() == 0)
        {
            while(1);
        }
        while(1);
    }
    else
    {
        while(1);
    }
    return 0;
}