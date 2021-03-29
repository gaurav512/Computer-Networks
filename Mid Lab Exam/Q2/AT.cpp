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
#define BUFF_LEN 1024
#define MAXCLIENTS 10
#define UNIXSTR_PATH1 "/tmp/fd-pass.socket1"
#define UNIXSTR_PATH2 "/tmp/fd-pass.socket2"
#define UNIXSTR_PATH3 "/tmp/fd-pass.socket3"
#define QUALIFYING_SCORE 50

using namespace std;

int usfd[3];

ssize_t send_fd(int sfd, int fd)
{
    struct msghdr msg;
    char dup[256];
    struct iovec iov = {
        .iov_base = &dup,
        .iov_len = sizeof(dup)
    };

    struct cmsghdr *cmptr;
    char control[CMSG_SPACE(sizeof(int))];

    msg.msg_control = control;
    msg.msg_controllen = sizeof(control);
    msg.msg_name = NULL;
    msg.msg_namelen = 0;
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    cmptr = CMSG_FIRSTHDR(&msg);
    cmptr->cmsg_len = CMSG_LEN(sizeof(int));
    cmptr->cmsg_level = SOL_SOCKET;
    cmptr->cmsg_type = SCM_RIGHTS;
    *((int*)CMSG_DATA(cmptr)) = fd;

    return sendmsg(sfd, &msg, 0);
}


void *myThread(void* argvp)
{
    int fd = *(int *) argvp;
    
    int score = rand() % 101;      // Score for the candidate

    if(score >= QUALIFYING_SCORE)
    {
        char msg[] = "You HAVE qualified the Aptitude Test\n";
        send(fd, msg, sizeof(msg), 0);

        for(int i = 0; i < 3; i++)
        {
            if(fork() == 0)
            {
                send_fd(usfd[i], fd);
                exit(1);
            }
        }    
    }
    else
    {
        char msg[] = "You HAVE NOT qualified the Aptitude Test\n";
        send(fd, msg, sizeof(msg), 0);
    }
    return NULL;
}

int main()
{
    // Creating 3 usfd for each R1, R2, R3

    for(int i = 0; i < 3; i++)
    {    
        if((usfd[i] = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
        {
            perror("socket FAILED");
            exit(1);
        }

        struct sockaddr_un udsaddr;
        udsaddr.sun_family = AF_UNIX;
        if(i == 0)
            strcpy(udsaddr.sun_path, UNIXSTR_PATH1);
        else if(i == 1)
            strcpy(udsaddr.sun_path, UNIXSTR_PATH2);
        else
            strcpy(udsaddr.sun_path, UNIXSTR_PATH3);
        
        if(connect(usfd[i], (struct sockaddr*) &udsaddr, sizeof(udsaddr)) < 0)
        {
            perror("connect FAILED");
            exit(1);
        }
    }
    

    int sfd;
    sockaddr_in at_addr;
    if((sfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket FAILED");
        exit(1);
    }

    int opt = 1;
    if(setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt FAILED");
        exit(EXIT_FAILURE);
    }

    at_addr.sin_family = AF_INET;
    at_addr.sin_addr.s_addr = INADDR_ANY;
    at_addr.sin_port = htons(PORT);
    
    if(bind(sfd, (sockaddr*) &at_addr, sizeof(at_addr)) < 0)
    {
        perror("bind FAILED");
        exit(1);
    }

    if(listen(sfd, MAXCLIENTS) < 0)
    {
        perror("listen FAILED");
        exit(1);
    }
    
    while(1)
    {
        int nsfd;
        struct sockaddr_in c_addr;
        int len = sizeof(c_addr);
        if((nsfd = accept(sfd, (struct sockaddr*) &c_addr, (socklen_t*)&len)) < 0)
        {
            perror("accept FAILED");
            exit(1);
        }
        cout << "New connection accpeted by server. Socket fd: " << nsfd << ", IP address: " <<
                    inet_ntoa(c_addr.sin_addr) << ", Port: " << ntohs(c_addr.sin_port) << '\n';

        pthread_t tid;
        pthread_create(&tid, NULL, myThread, &nsfd);
    
    }


    return 0;
}