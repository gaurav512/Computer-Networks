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


void *threadFunction(void* args)
{
    int key = ftok("shm", 65);
    int shmid = shmget(key, sizeof(int), 0666 | IPC_CREAT);
    int *shmval = (int*)shmat(shmid, NULL, 0);

    kill(*shmval, SIGUSR1);
    
    while(1);
}

int main()
{
    pthread_t tid;
    pthread_create(&tid, NULL, threadFunction, NULL);
    pthread_join(tid, NULL); 
    return 0;
}