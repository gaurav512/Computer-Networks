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
    signal(SIGUSR1, handler);
    
    printf("Enter message for P1: ");
    char buffer[100];
    scanf("%s", buffer);
    write(Yfd, buffer, sizeof(buffer));
    memset(buffer, 0, sizeof(buffer));

    kill(pidP1, SIGUSR2);

    while(!read(Xfd, buffer, sizeof(buffer)));
    printf("P1 says: %s\n", buffer);

}

int main()
{
    signal(SIGUSR1, handler);

    char *X = "/tmp/x_fifo";
    char *Y = "/tmp/y_fifo";
    mkfifo(X, 0666);
    mkfifo(Y, 0666);
    Xfd = open(X, O_RDONLY);
    Yfd = open(Y, O_WRONLY);

    sleep(2);

    pidP2 = getpid();
    while(!read(Xfd, &pidP1, sizeof(pid_t)));
    write(Yfd, &pidP2, sizeof(pid_t));

    while(1);
    return 0;
}