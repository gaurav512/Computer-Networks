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

int main()
{
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    read(0, buffer, sizeof(buffer));

    string str(buffer);
    
    stringstream ss(str);
    int res = 0;
    string temp;
    while(ss >> temp) 
    {
        res += stoi(temp);
    }

    temp = to_string(res);
    
    memset(buffer, 0, sizeof(buffer));
    strcpy(buffer, temp.c_str());
    write(1, buffer, sizeof(buffer));

    return 0;
}