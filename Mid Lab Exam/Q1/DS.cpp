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

#define PORT1 8080
#define PORT2 8081
#define BUFF_LEN 1024
#define MAXCLIENTS 2
#define UNIXSTR_PATH "/tmp/fd-pass.socket"
#define N 2

using namespace std;

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

    // Making UnixDomain socket
    int usfd;
    if((usfd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
    {
        perror("socket FAILED");
        exit(1);
    }
    

    struct sockaddr_un udsaddr;
    udsaddr.sun_family = AF_UNIX;
    strcpy(udsaddr.sun_path, UNIXSTR_PATH);

    if(connect(usfd, (struct sockaddr*) &udsaddr, sizeof(udsaddr)) < 0)
    {
        perror("connect FAILED");
        exit(1);
    }
    


    int sfd[N];
    sockaddr_in sockaddress[N];
    int ports[N] = {PORT1, PORT2};
    for(int i = 0; i < N; i++)
    {
        if((sfd[i] = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        {
            perror("socket FAILED");
            exit(1);
        }

        int opt = 1;
        if(setsockopt(sfd[i], SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
        {
            perror("setsockopt FAILED");
            exit(EXIT_FAILURE);
        }

        sockaddress[i].sin_family = AF_INET;
        sockaddress[i].sin_addr.s_addr = INADDR_ANY;
        sockaddress[i].sin_port = htons(ports[i]);
        
        if(bind(sfd[i], (sockaddr*) &sockaddress[i], sizeof(sockaddress[i])) < 0)
        {
            perror("bind FAILED");
            exit(1);
        }

        if(listen(sfd[i], 5) < 0)
        {
            perror("listen FAILED");
            exit(1);
        }
    }


    int s1pid;
    // Getting S1's pid
    mkfifo("/tmp/myfifo", 0666);
    int fifo_fd = open("/tmp/myfifo", O_RDONLY);
    char s1pidarr[BUFF_LEN];
    while(!read(fifo_fd, s1pidarr, BUFF_LEN));
    s1pid = atoi(s1pidarr);
    cout << "S1's pid is: " << s1pid;
    fflush(stdout);


    fd_set rset;
    int max_fd;

    while(1)
    {
        FD_ZERO(&rset);
        for(int i = 0; i < 3; i++)
        {
            FD_SET(sfd[i], &rset);
        }

        max_fd = max(sfd[0], sfd[1]);

        struct timeval t;
        t.tv_sec = 3;
        t.tv_usec = 500;
        
        int activity;
        if((activity = select(max_fd+1, &rset, NULL, NULL, &t)) < 0)
        {
            perror("select FAILED");
            exit(1);
        }

        if(FD_ISSET(sfd[0], &rset))
        {
            cout << "sfd1 is set\n";

            int nsfd;
            struct sockaddr_in c_addr;
            int len = sizeof(c_addr);
            if((nsfd = accept(sfd[0], (struct sockaddr*) &c_addr, (socklen_t*)&len)) < 0)
            {
                perror("accept FAILED");
                exit(1);
            }
            cout << "New connection accpeted by server. Socket fd: " << nsfd << ", IP address: " <<
                        inet_ntoa(c_addr.sin_addr) << ", Port: " << ntohs(c_addr.sin_port) << '\n';
            close(nsfd);

            kill(s1pid, SIGUSR1);

        }

        if(FD_ISSET(sfd[1], &rset))
        {
            cout << "sfd2 is set\n";

            int nsfd;
            struct sockaddr_in c_addr;
            int len = sizeof(c_addr);
            if((nsfd = accept(sfd[1], (struct sockaddr*) &c_addr, (socklen_t*)&len)) < 0)
            {
                perror("accept FAILED");
                exit(1);
            }
            cout << "New connection accpeted by server. Socket fd: " << nsfd << ", IP address: " <<
                        inet_ntoa(c_addr.sin_addr) << ", Port: " << ntohs(c_addr.sin_port) << '\n';

            if(send_fd(usfd, nsfd) < 0)
            {
                perror("send_fd FAILED");
                exit(1);
            }

            cout << "fd sent to S2 successfully\n\n";

            close(nsfd);
        }

        
    }




}