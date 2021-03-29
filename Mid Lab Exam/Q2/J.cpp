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
#include<map>

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

#define PORT 9090
#define BUFF_LEN 1024
#define MAXCLIENTS 2
#define N 2

using namespace std;

map<string, int>scores;

int main()
{
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
        char _name[BUFF_LEN], _score[BUFF_LEN];

        read(nsfd, _name, BUFF_LEN);
        read(nsfd, _score, BUFF_LEN);
        
        int score = stoi(_score);
        string name(_name);
        scores[name] += score;
    }

    
    string selected = "";
    int c = 0;
    for(auto itr = scores.rbegin(); itr != scores.rend(); itr++)
    {
        selected += itr->first + '\n';
        c++;
        if(c == 0)
            break;
    } 
    
    cout << "The selected candidates are:\n" << selected;

    return 0;
}