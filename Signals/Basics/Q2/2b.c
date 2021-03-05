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


int main()
{
    int c = 0;
    while(1)
    {
        c++;
        printf("%d\n", c);
        if(c == 10)
        {
            // fflush(stdout);
            printf("Terminating now!");
            fflush(stdout);
            kill(getpid(), SIGKILL);
        }
    }
    return 0;
}