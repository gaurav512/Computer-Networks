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


int main()
{
    key_t key;
    key = ftok("shm", 64);
    int shmid = shmget(key, sizeof(int), IPC_CREAT | 0666);
    int *shmval = (int *) shmat(shmid, NULL, 0);
    int pid = *shmval;

    killpg(getpgid(pid), SIGUSR1);  // Sending signal to process group of Parent process

    return 0;
}