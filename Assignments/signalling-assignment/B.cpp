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

char* shmvalue;

void handler(int signum)
{
    printf("B says: %s\n", shmvalue);
    // printf("B\n");
    fflush(stdout);
}


int main()
{
    signal(SIGUSR1, handler);
    sleep(1);
    key_t key;
    int shmid, *shmval;
    key = ftok("shareWithG1", 65);
    shmid = shmget(key, sizeof(int), 0666 | IPC_CREAT);
    shmval = (int *)shmat(shmid, NULL, 0);
    
    int pidA = *shmval;
    setpgid(getpid(), pidA);
    
    *shmval = pidA;

    key_t key1;
    int shmid1;
    key1 = ftok("IPC", 65);
    shmid1 = shmget(key1, 1000, 0666 | IPC_CREAT);
    shmvalue = (char *)shmat(shmid1, NULL, 0);

    while(1);
}