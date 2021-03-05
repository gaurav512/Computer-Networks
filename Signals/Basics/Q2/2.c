// Does not terminate the process the first time we do ctrl+c

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

int cnt = 0;
void handler(int signum)
{
    cnt++;
    printf("Ctrl+C-ing %d times, I won't terminate\n", cnt);
    if(cnt == 6)
    {
        printf("Ctrl+C-ing %d times, I WILL terminate\n", cnt);
        kill(getpid(), SIGKILL);
    }
}

int main()
{
    signal(SIGINT, handler);
    while(1);
    return 0;
}