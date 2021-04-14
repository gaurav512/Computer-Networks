#include<iostream>
#include<bits/stdc++.h>
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

using namespace std;

struct service {
    string name;
    string type;
    int port;
    int max_requests;
    string process_exec;
};

int main()
{
    cout << "Internet Super Server is running...\n";

    cout << "Loading config file info...\n";

    ifstream fin;
    fin.open("config.txt");
    string s, item;

    vector<service>services;

    while(getline(fin, s))
    {
        service new_service;
        stringstream ss(s);
        ss >> item;
        new_service.name = item;
        ss >> item;
        new_service.type = item;
        ss >> item;
        new_service.port = stoi(item);
            
        if(new_service.type == "TCP")
        {
            ss >> item;
            new_service.max_requests = stoi(item);
            ss >> item;    
            new_service.process_exec = item;
        }
        services.push_back(new_service);
        
    }
    fin.close();

    int sz = services.size();
    cout << "Initializing the socket sfds as per the config file...\n";
    vector<int>sfd(sz);
    vector<struct sockaddr_in> address(sz);


    for(int i = 0; i < sz; i++)
    {
        if(services[i].type == "TCP")
        {
            if((sfd[i] = socket(AF_INET, SOCK_STREAM, 0)) < 0)
            {
                perror("TCP socket FAILED");
                exit(EXIT_FAILURE);
            }
            
            int opt = 1;
            if(setsockopt(sfd[i], SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
            {
                perror("setsockopt FAILED");
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            if((sfd[i] = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
            {
                perror("UDP socket FAILED");
                exit(EXIT_FAILURE);
            }
        }

        address[i].sin_family = AF_INET;
        address[i].sin_addr.s_addr = INADDR_ANY;
        address[i].sin_port = htons(services[i].port);

        if(bind(sfd[i], (struct sockaddr*) &address[i], sizeof(address[i])) < 0)
        {
            perror("bind FAILED");
            exit(EXIT_FAILURE);
        }

        if(services[i].type == "TCP")
        {
            if(listen(sfd[i], services[i].max_requests) < 0)
            {
                perror("listen FAILED");
                exit(EXIT_FAILURE);
            }
        }
    }
    
    
    cout << "\nWaiting for connections...\n";
    fd_set readset;

    // select system call
    while(1)
    {
        FD_ZERO(&readset);
        int max_fd = 0;
        for(int i = 0; i < sz; i++)
        {
            max_fd = max(max_fd, sfd[i]);
            FD_SET(sfd[i], &readset);
        }

                struct timeval t;
        t.tv_sec = 3;
        t.tv_usec = 500;
        
        int activity;
        if((activity = select(max_fd+1, &readset, NULL, NULL, &t)) < 0)
        {
            perror("select FAILED");
            exit(1);
        }

        for(int i = 0; i < sz; i++)
        {
            if(FD_ISSET(sfd[i], &readset))
            {
                if(services[i].type == "TCP")
                {
                    cout << "Accepting TCP connection for service: [" << services[i].name << "]...\n";
                    
                    int nsfd;
                    struct sockaddr_in c_addr;
                    int len = sizeof(c_addr);
                    if((nsfd = accept(sfd[i], (struct sockaddr*) &c_addr, (socklen_t*)&len)) < 0)
                    {
                        perror("accept FAILED");
                        exit(EXIT_FAILURE);
                    }

                    cout << "Connection accepted!\n";
                    int pid = fork();
                    
                    if(pid == 0)
                    {
                        close(sfd[i]);
                        dup2(nsfd, 0);
                        dup2(nsfd, 1);
                        
                        char p[1024];
                        strcpy(p, services[i].process_exec.c_str());
                        char *args[] = {p,NULL}; 
                        execv(args[0], args);
                    }
                    else
                    {
                        close(nsfd);
                    }
                }
                else
                {
                    cout << "Accepting UDP connection for service: [" << services[i].name << "]...\n";

                    struct sockaddr_in c_addr;
                    int len = sizeof(c_addr);

                    char buffer[1024];
                    memset(buffer, 0, sizeof(buffer));
                    int ret;

                    if((ret = recvfrom(sfd[i], buffer, sizeof(buffer), 0, (struct sockaddr*)&c_addr, (socklen_t *)&len)) < 0)
                    {
                        perror("recvfrom FAILED");
                        exit(EXIT_FAILURE);
                    }

                    cout << "Received UDP client message\n";
                    cout << "Processing the received message...\n";

                    for(int i = 0; i < ret; i++) 
                        if(isupper(buffer[i]))
                            buffer[i] = tolower(buffer[i]);
                        else if(islower(buffer[i]))
                            buffer[i] = toupper(buffer[i]);
                        
                    cout << "Sending the serviced response to the client...\n";
                    fflush(stdout);
                    if(sendto(sfd[i], buffer, sizeof(buffer), 0, (struct sockaddr*)&c_addr, sizeof(c_addr)) < 0)
                    {
                        perror("sendto FAILED");
                        exit(EXIT_FAILURE);
                    }
                    fflush(stdout);
                    cout << "Response sent successfully...\n";
                }
                
            }
        }
    }
    

    return 0;
}