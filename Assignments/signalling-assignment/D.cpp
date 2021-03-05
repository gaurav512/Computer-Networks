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
    // Getting pid of S
    key_t key = ftok("shareWithD", 65);
    int shm = shmget(key, sizeof(int), 0666 | IPC_CREAT);
    int *shmval = (int *)shmat(shm, NULL, 0);
    // sleep(1);
    int pid = *shmval;
    

    while(1)
    {
        char buffer[100];
        printf("Type 1 to change the signal: ");
        scanf("%s", buffer);

        if(buffer[0] == '1')
            kill(pid, SIGUSR1);
        
        memset(buffer, 0, sizeof(buffer));
    }


    return 0;
}