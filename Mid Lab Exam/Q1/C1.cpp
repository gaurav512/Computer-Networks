#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<unistd.h>
#include<pthread.h>
#include<semaphore.h>
#include<fcntl.h>
#include<string.h>
#include<ctype.h>
#include<signal.h>
#include<poll.h>

#include<sys/wait.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/shm.h>
#include<sys/msg.h>
#include<sys/uio.h>
#include<sys/socket.h>
#include<sys/errno.h>
#include<sys/un.h>

#include<netinet/if_ether.h>
#include<net/ethernet.h>
#include<netinet/ether.h>
#include<netinet/udp.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<netinet/ip.h>

#define PORT 8080
#define PORT2 8082
#define BUFF_LEN 1024
#define MAXCLIENTS 5

using namespace std;

int main()
{
    int dsfd;

    if((dsfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket FAILED");
        exit(1);
    }

    struct sockaddr_in ds_addr;
    ds_addr.sin_family = AF_INET;
    ds_addr.sin_addr.s_addr = INADDR_ANY;
    ds_addr.sin_port = htons(PORT);

    if(connect(dsfd, (struct sockaddr*) &ds_addr, sizeof(ds_addr)) < 0)
    {
        perror("connect FAILED");
        exit(1);
    }

    sleep(1);

    int s1fd;
        if((s1fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket FAILED");
        exit(1);
    }

    struct sockaddr_in s1_addr;
    s1_addr.sin_family = AF_INET;
    s1_addr.sin_addr.s_addr = INADDR_ANY;
    s1_addr.sin_port = htons(PORT2);

    if(connect(s1fd, (struct sockaddr*) &s1_addr, sizeof(s1_addr)) < 0)
    {
        perror("connect FAILED");
        exit(1);
    }

    char priority[2];
    memset(priority, 0, 2);
    int p;
    printf("Enter priority: ");
    scanf("%d", &p);
    priority[0] = p+'0';
    send(s1fd, priority, 2, 0);


    return 0;
}
