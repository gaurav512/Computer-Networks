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

pthread_t tid1, tid2;

void handler(int signum)
{
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);
    int s = pthread_sigmask(SIG_UNBLOCK, &set, NULL);

    sleep(2);
    pthread_kill(tid2, SIGUSR1);
}

void handler2(int signum)
{
    printf("Signal %d received by thread T2\n", signum);
    exit(0);
}

void* thread1(void* arg)
{
    printf("Thread1 created\n");

    sigset_t *set = (sigset_t *)arg;

    int sig;
    sigwait(set, &sig);
    pthread_kill(tid2, SIGUSR2);
    printf("Signal handling thread (Thread1) received signal %d\n", sig);
    return NULL;
}

void* thread2(void* arg)
{
    printf("Thread2 created\n");
    while(1);
    return NULL;
}

int main()
{
    int key = ftok("shm", 65);
    int shmid = shmget(key, sizeof(int), 0666 | IPC_CREAT);
    int *shmval = (int*)shmat(shmid, NULL, 0);
    *shmval = getpid();

    signal(SIGUSR2, handler);
    signal(SIGUSR1, handler2);

    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);
    pthread_sigmask(SIG_BLOCK, &set, NULL);

    sleep(2);

    pthread_create(&tid2, NULL, thread2, NULL);
    pthread_create(&tid1, NULL, thread1, &set);
    
    while(1);
    shmdt(shmval);
    shmctl(shmid, IPC_RMID, NULL);


    return 0;
}