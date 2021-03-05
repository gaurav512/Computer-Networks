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
#include<poll.h>

using namespace std;

#define POLLIN 0x001


int pgid_G1, pgid_G2;
bool is_G1_active = false;
int c1fd, c2fd;

void handlerForD(int signum)
{
    // printf("Hey");
    fflush(stdout);
    if(is_G1_active)
    {
        killpg(pgid_G1, SIGSTOP);
        killpg(pgid_G2, SIGCONT);
    }
    else
    {
        killpg(pgid_G2, SIGSTOP);
        killpg(pgid_G1, SIGCONT);
    }
    is_G1_active = !is_G1_active; 
}

int main()
{
    key_t key;
    int shmid, *shmval;

    // Sharing pid with D
    key = ftok("shareWithD", 65);
    shmid = shmget(key, sizeof(int), 0666 | IPC_CREAT);
    shmval = (int *)shmat(shmid, NULL, 0);
    *shmval = getpid(); 

    signal(SIGUSR1, handlerForD);

    // Opening fifo for C1
    mkfifo("/tmp/C1fifo", 0666);
    c1fd = open("/tmp/C1fifo", O_RDONLY);

    // Opening fifo for C2
    mkfifo("/tmp/C2fifo", 0666);
    c2fd = open("/tmp/C2fifo", O_RDONLY);

    // Opening child processes
    FILE *A, *B, *S1, *S2;
    A = popen("./A", "w");
    B = popen("./B", "w");
    
    sleep(2);

    key_t key1;
    int shmid1, *shmval1;
    key1 = ftok("shareWithG1", 65);
    shmid1 = shmget(key1, sizeof(int), 0666 | IPC_CREAT);
    shmval1 = (int *)shmat(shmid1, NULL, 0);
    pgid_G1 = *shmval1;

    sleep(3);

    S1 = popen("./S1", "w");
    S2 = popen("./S2", "w");
        
    sleep(2);

    key1 = ftok("shareWithG2", 65);
    shmid1 = shmget(key1, sizeof(int), 0666 | IPC_CREAT);
    shmval1 = (int *)shmat(shmid1, NULL, 0);
    pgid_G2 = *shmval1;

    // printf("G1 = %d\nG2 = %d\n", pgid_G1, pgid_G2);

    raise(SIGUSR1);

    // Creating an SHM for IPC between S and the children :)
    key1 = ftok("IPC", 65);
    shmid1 = shmget(key1, 1000, 0666 | IPC_CREAT);
    char* shmvalue = (char *)shmat(shmid1, NULL, 0);



    // Polling for input
    struct pollfd pfds[2];
    char buffer[1000];
    
    while(1) 
    {
        pfds[0].fd = c1fd;
        pfds[0].events = POLLIN;
        pfds[1].fd = c2fd;
        pfds[1].events = POLLIN;
        
        int r = poll(pfds, 2, -1);
        for(int i=0; i<2; i++)
        {
            if(pfds[i].revents & POLLIN)
            {
                read(pfds[i].fd, buffer, sizeof(buffer));
            
                // shmvalue = &buffer[0];
                strncpy(shmvalue, buffer, sizeof(buffer));
                killpg(pgid_G1, SIGUSR1);
                killpg(pgid_G2, SIGUSR1);
                memset(buffer, 0, sizeof(buffer));
            }
        }
    }
    return 0;    
}