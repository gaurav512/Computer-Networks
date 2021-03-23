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


int main()
{
    int usfd;
    if((usfd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
    {
        perror("socket FAILED");
        exit(1);
    }

    // unlink(UNIXSTR_PATH);

    struct sockaddr_un addr;
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, UNIXSTR_PATH);

    if(connect(usfd, (struct sockaddr*) &addr, sizeof(addr)) < 0)
    {
        perror("connect FAILED");
        exit(1);
    }

    int fd;
    if((fd = open("file.txt", O_RDONLY)) < 0)
    {
        perror("File open FAILED");
        exit(1);
    }

    char ch;
    
    int cnt = 0;
    char buffer[32];
    memset(buffer, 0 , sizeof(buffer));
    while(read(fd, &ch, sizeof(ch)) && cnt < 16)
    {
        buffer[cnt++] = ch;
    }


    std::cout << "<Read the following from file>\n\n" << buffer << '\n';

    send_fd(usfd, fd);

    return 0;
}