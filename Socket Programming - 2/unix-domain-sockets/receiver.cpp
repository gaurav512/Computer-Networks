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
#define MAXCLIENTS 20

using namespace std;

#define UNIXSTR_PATH "/tmp/fd-pass.socket"

ssize_t read_fd(int nusfd, int &fd)
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

    ssize_t n;
    if((n = recvmsg(nusfd, &msg, 0)) < 0)
    {
        perror("recvmsg FAILED");
        exit(1);
    }

    if(!n)
        return n;
    
    if((cmptr = CMSG_FIRSTHDR(&msg)) != NULL && cmptr->cmsg_len == CMSG_LEN(sizeof(int)))
    {
        if(cmptr->cmsg_level != SOL_SOCKET)
        {
            perror("control level != SOL_SOCKET");
            exit(1);
        }
        if(cmptr->cmsg_type != SCM_RIGHTS)
        {
            perror("control type != SCM_RIGHTS");
            exit(1);
        }
        fd = (int)(*CMSG_DATA(cmptr));
    }
    else
    {
        fd = -1;
    }
    return n;
}


int main()
{
    int usfd;
    if((usfd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
    {
        perror("socket FAILED");
        exit(1);
    }

    unlink(UNIXSTR_PATH);

    struct sockaddr_un self_addr, other_addr;
    self_addr.sun_family = AF_UNIX;
    strcpy(self_addr.sun_path, UNIXSTR_PATH);

    if(bind(usfd, (struct sockaddr*) &self_addr, sizeof(self_addr)) < 0)
    {
        perror("bind FAILED");
        exit(1);
    }

    if(listen(usfd, MAXCLIENTS) < 0)
    {
        perror("listen FAILED");
        exit(1);
    }

    int nusfd;
    int other_len = sizeof(other_addr);
    if((nusfd = accept(usfd, (struct sockaddr*) &other_addr, (socklen_t *) &other_len)) < 0)
    {
        perror("accept FAILED");
        exit(1);
    }
    else
    {
        cout << "Connection successful\n";
    }

    int fd;

    while(!read_fd(nusfd, fd));

    if(fd < 0)
    {
        perror("fd transfer FAILED");
        exit(1);
    }

    char buffer[BUFF_LEN];
    memset(buffer, 0, BUFF_LEN);
    cout << "<Read the following from file:>\n\n";
    while(read(fd, buffer, BUFF_LEN))
    {
        cout << buffer;
        memset(buffer, 0, BUFF_LEN);
    }

    cout << "\n<Read successful>\n";
    
    return 0;
}