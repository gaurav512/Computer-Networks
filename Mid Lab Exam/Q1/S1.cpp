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

#define PORT 8082
#define BUFF_LEN 1024
#define MAXCLIENTS 2
#define N 2

using namespace std;

int nsfd;

void *myThread(void* vargp)
{
    int fd = *(int *)vargp;
    while(1)
    {
        char buffer[BUFF_LEN];
        memset(buffer, 0, BUFF_LEN);
        read(fd, buffer, BUFF_LEN);
        cout << "Client says: " << buffer << '\n';
    }
}


void handler(int signum)
{

    int sfd;
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

    struct sockaddr_in saddr;

    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = INADDR_ANY;
    saddr.sin_port = htons(PORT);
    
    if(bind(sfd, (sockaddr*) &saddr, sizeof(saddr)) < 0)
    {
        perror("bind FAILED");
        exit(1);
    }

    if(listen(sfd, 5) < 0)
    {
        perror("listen FAILED");
        exit(1);
    }

    struct sockaddr_in c_addr;
    int len = sizeof(c_addr);
    if((nsfd = accept(sfd, (struct sockaddr*) &c_addr, (socklen_t*)&len)) < 0)
    {
        perror("accept FAILED");
        exit(1);
    }
    cout << "New connection accpeted by server. Socket fd: " << nsfd << ", IP address: " <<
                inet_ntoa(c_addr.sin_addr) << ", Port: " << ntohs(c_addr.sin_port) << '\n';
    
    char priority[2];
    read(nsfd, priority, 2);

    if(priority[0] == '1')
    {
        while(1)
        {
            char buffer[BUFF_LEN];
            memset(buffer, 0, BUFF_LEN);
            read(nsfd, buffer, BUFF_LEN);
            cout << "Client says: " << buffer << '\n';
        }
    }
    else if(priority[0] == '2')
    {
        pthread_t tid;
        pthread_create(&tid, NULL, myThread, (void*)&nsfd);
        pthread_join(tid, NULL);
    }

}

int main()
{
    // Writing S1's pid to the fifo

    int s1pid = getpid();
    cout << "S1's pid is: " << s1pid;
    mkfifo("/tmp/myfifo", 0666);
    int fifo_fd = open("/tmp/myfifo", O_WRONLY);
    string s1pid_str = to_string(s1pid);
    while(!write(fifo_fd, s1pid_str.c_str(), strlen(s1pid_str.c_str())));

    signal(SIGUSR1, handler);

    while(1);

    return 0;
}