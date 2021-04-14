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

#define PORT 8080
#define BUFLEN 1024

int main()
{
    int sfd;
	char buf[BUFLEN];
	struct sockaddr_in server_addr;
	
	cout << "TCP_CLIENT1 started...\n";
	
	if ((sfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		perror("socket FAILED");
		exit(EXIT_FAILURE);
	}
	
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(PORT);
	
	if (connect(sfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
		perror("connect FAILED");
        exit(EXIT_FAILURE);
	}
	
	char rbuf[BUFLEN],wbuf[BUFLEN];
	
	string s;
	cout << "Enter a string: ";
	fflush(stdout);
    getline(cin,s);
	memset(wbuf, 0, sizeof(wbuf));
	memset(rbuf, 0, sizeof(rbuf));
	strcpy(wbuf, s.c_str());
	
    cout << "Sending to server...\n";
    cout << write(sfd, wbuf, sizeof(wbuf));
    cout << "Message sent to server!\n";

	while(!read(sfd, rbuf, sizeof(rbuf)));
    cout << "Message received from server\n";

	for(int i = 0 ; i < sizeof(rbuf) ; i++){
		cout <<rbuf[i];
	}
	cout << "\n";
    fflush(stdout);
    
	return 0;	
}