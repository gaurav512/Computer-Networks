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

#define PORT 8081
#define BUFLEN 1024

int main()
{
    int sfd;
	cout << "UDP client started...\n";
	
	struct sockaddr_in server_addr;
	
	if((sfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
		perror("socket FAILED");
		exit(EXIT_FAILURE);
	}
	
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(PORT);
	
	char buffer[BUFLEN];
    memset(buffer, 0, sizeof(buffer));
    fflush(stdout);
	cout << "Enter a string: ";
	fflush(stdout);
	string s;
    cin >> s;
	
	memset(buffer, 0, sizeof(buffer));
	strcpy(buffer, s.c_str());
	
    cout << "Sending message to server...\n";
	sendto(sfd, (char *)buffer, sizeof(buffer), MSG_CONFIRM, (struct sockaddr *)&server_addr,sizeof(server_addr));
	cout << "Message to server sent!\n";
    
    int len;
    char res[BUFLEN];
    memset(res, 0, sizeof(res));

    cout << "Receiving message from server...\n";
	recvfrom(sfd,(char *)res, sizeof(res),MSG_WAITALL,(struct sockaddr *)&server_addr,(socklen_t *)&len);
	
    for(int i = 0 ; i < sizeof(res) ; i++)
		cout << res[i];
	cout << "\n";
	fflush(stdout);
	return 0;   
}