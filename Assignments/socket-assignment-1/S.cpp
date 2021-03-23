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

#define PORT1 9080
#define PORT2 9081
#define PORT3 9082
#define BUFF_LEN 1024
#define MAXCLIENTS 5

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
    // Creating a Unix-domain socket
    int usfd;
    if((usfd = socket(AF_UNIX, SOCK_STREAM, 0 )) < 0)
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




    // sfd[0] -> sfd1
    // sfd[1] -> sfd2
    // sfd[2] -> sfd3
    int sfd[3];

    // Creating sockets
    if((sfd[0] = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket FAILED");
        exit(1);
    }    
    if((sfd[1] = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket FAILED");
        exit(1);
    }    
    if((sfd[2] = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket FAILED");
        exit(1);
    }    
    
    // Forcefully attaching socket to the port
    for(int i = 0; i < 3; i++)
    {
        int opt = 1;
        if (setsockopt(sfd[i], SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
        {
            perror("setsockopt");
            exit(EXIT_FAILURE);
        }
    }


    // socket address info
    struct sockaddr_in addr[3];
    int addrlen = sizeof(sockaddr_in);
    int port[3] = {PORT1, PORT2, PORT3};

    for(int i = 0; i < 3; i++)
    {
        addr[i].sin_family = AF_INET;
        addr[i].sin_addr.s_addr = INADDR_ANY;
        addr[i].sin_port = htons(port[i]);
    }

    // binding to socket
    for(int i = 0; i < 3; i++)
    {
        if(bind(sfd[i], (struct sockaddr*) &addr[i], addrlen) < 0)
        {
            perror("bind FAILED");
            exit(1);
        }
    }

    if(listen(sfd[0], MAXCLIENTS) < 0)
    {
        perror("listen FAILED");
        exit(1);
    }
    if(listen(sfd[2], MAXCLIENTS) < 0)
    {
        perror("listen FAILED");
        exit(1);
    }

    fd_set rset;
    int max_fd;

    cout << "Server is waiting for connections.....\n\n";
    
    while(1)
    {
        FD_ZERO(&rset);
        for(int i = 0; i < 3; i++)
        {
            FD_SET(sfd[i], &rset);
        }

        max_fd = max(sfd[0], max(sfd[1], sfd[2]));

        struct timeval t;
        t.tv_sec = 3;
        t.tv_usec = 500;
        
        int activity;
        if((activity = select(max_fd+1, &rset, NULL, NULL, &t)) < 0)
        {
            perror("select FAILED");
            exit(1);
        }

        for(int i = 0; i < 3; i++)
        {
            if(i == 1)
                continue;
            
            if(FD_ISSET(sfd[i], &rset))
            {
                cout << "sockfd is set\n";
                
                int nsfd;
                struct sockaddr_in c_addr;
                int len = sizeof(c_addr);
                if((nsfd = accept(sfd[i], (struct sockaddr*) &c_addr, (socklen_t*)&len)) < 0)
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

        if(FD_ISSET(sfd[1], &rset))
        {
            cout << "Connected to a UDP client\n";
            char buffer[BUFF_LEN];
            memset(buffer, 0, BUFF_LEN);
            struct sockaddr_in c_addr;
            int len = sizeof(c_addr);
            recvfrom(sfd[1], buffer, BUFF_LEN, MSG_WAITALL, (struct sockaddr *) &c_addr, (socklen_t *) &len);
            cout << "Message read by server is: " << buffer << "\n\n";
        }

    }

}