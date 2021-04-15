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
#include<netinet/tcp.h>

#define PORT1 8080
#define PORT2 9090
#define PORT3 7070

#define N 3
#define BUFFER_LENGTH 1024

using namespace std;

int main()
{
    int sfd[N];
    int nsfd[N];
    sockaddr_in sock_addr[N];
    socklen_t addr_len[N];
    int PORT[N] = {PORT1, PORT2, PORT3};

    pollfd pfds[N];

    cout << "Server is running...\n\n";

    for (int i = 0; i < N; i++)
    {
        addr_len[i] = sizeof(sock_addr[i]);
        sock_addr[i].sin_family = AF_INET;
        sock_addr[i].sin_addr.s_addr = INADDR_ANY;
        sock_addr[i].sin_port = htons(PORT[i]);

        if ((sfd[i] = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        {
            perror("socket FAILED");
            exit(EXIT_FAILURE);
        }

        if (bind(sfd[i], (sockaddr *)&sock_addr[i], sizeof(sock_addr[i])) < 0)
        {
            perror("bind FAILED");
            exit(EXIT_FAILURE);
        }

        if (listen(sfd[i], 5) < 0)
        {
            perror("listen FAILED");
            exit(EXIT_FAILURE);
        }

        pfds[i].fd = sfd[i];
        pfds[i].events = POLLIN;
    }

    while (1)
    {
        int p = poll(pfds, 4, 1000);
        if (p != 0)
        {
            if (p == -1)
            {
                perror("poll FAILED");
                exit(EXIT_FAILURE);
            }
            for (int i = 0; i < N; i++)
            {
                if (pfds[i].revents & POLLIN)
                {
                    nsfd[i] = accept(sfd[i], (sockaddr *)&sock_addr[i], &addr_len[i]);
                    pid_t pid = fork();
                    if (pid == 0)
                    {
                        cout << "Accepted connection on sfd" << i+1 << "...\n";
                        char read_buffer[BUFFER_LENGTH];
                        char write_buffer[BUFFER_LENGTH] = "Hello, I am server!";

                        cout << "Waiting for client's message...\n";
                        recv(nsfd[i], read_buffer, sizeof(read_buffer), 0);
                        cout << "Client says: " << read_buffer << "\n\n";
                        
                        cout << "Sending message to client....\n";
                        send(nsfd[i], write_buffer, sizeof(write_buffer), 0);
                        cout << "Sent message to client!\n\n";
                    }
                    else
                    {
                        close(nsfd[i]);
                    }
                }
            }
        }
    }
}