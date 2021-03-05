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
#include<sys/ipc.h>
#include<sys/shm.h>
#include<ctype.h>
#include<signal.h>

int Xfd, Yfd;
pid_t pidP1, pidP2;

void handler(int signum)
{
    signal(SIGUSR2, handler);
    
    char buffer[100];
    while(!read(Yfd, buffer, sizeof(buffer)));
    printf("P2 says: %s\n", buffer);
    memset(buffer, 0, sizeof(buffer));

    kill(pidP2, SIGUSR1);

    printf("Enter message for P2: ");
    scanf("%s", buffer);
    write(Xfd, buffer, sizeof(buffer));
}

int main()
{
    signal(SIGUSR2, handler);

    char *X = "/tmp/x_fifo";
    char *Y = "/tmp/y_fifo";
    mkfifo(X, 0666);
    mkfifo(Y, 0666);
    Xfd = open(X, O_WRONLY);
    Yfd = open(Y, O_RDONLY);

    sleep(2);

    pidP1 = getpid();   // Getting pid of p1
    write(Xfd, &pidP1, sizeof(pid_t));  // Writing pid of p1 to xfifo
    while(!read(Yfd, &pidP2, sizeof(pid_t)));   // Reading pid of p2 from yfifo

    kill(pidP2, SIGUSR1);

    while(1);
    return 0;
}