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

using namespace std;

struct sockaddr_in source_addr, dest_addr;

unsigned short csum(unsigned short *ptr, int nbytes) 
{
	register long sum;
	unsigned short oddbyte;
	register short answer;

	sum = 0;

	while(nbytes > 1) 
    {
		sum += *ptr++;
		nbytes -= 2;
	}
	if(nbytes == 1)
    {
		oddbyte = 0;
		*((u_char*) &oddbyte) = *(u_char*)ptr;
		sum += oddbyte;
	}

	sum = (sum>>16) + (sum & 0xffff);
	sum = sum + (sum>>16);
	answer = (short)~sum;
	
	return answer;
}


void printIPHeader(struct iphdr* ip) {
    cout<<"------------------------\n";
	cout<<"*** Printing IP header ***\n";
	cout<<" IP version: " << (unsigned int)ip->version << endl;
	cout<<" IP header length: " << (unsigned int)ip->ihl << endl;
	cout<<" Type of service: " << (unsigned int)ip->tos << endl;
	cout<<" Total IP packet length: " << ntohs(ip->tot_len) << endl;
	cout<<" Packet ID: " << ntohs(ip->id) << endl;
	cout<<" Time to Live: " << (unsigned int) ip->ttl << endl;
	cout<<" Protocol: " << (unsigned int)ip->protocol << endl;
	cout<<" Check: " << ip->check << endl;
	cout<<" Source IP: " << inet_ntoa(*(in_addr*)&ip->saddr) << endl;
	cout<<" Destination IP:" << inet_ntoa(*(in_addr*)&ip->daddr) << endl;
	cout<<"------------------------\n";
}

int main()
{
    // Creating Raw socket
    int rsfd;
    if((rsfd = socket(AF_INET, SOCK_RAW, 69)) < 0)
    {
        perror("socket FAILED");
        exit(EXIT_FAILURE);
    }

    // setsockopt
    int opt = 1;
	const int *val = &opt;
	if (setsockopt(rsfd, IPPROTO_IP, IP_HDRINCL, val, sizeof(opt)) < 0) 
    {
		perror("setsockopt FAILED");
		exit(EXIT_FAILURE);
	}	

    struct sockaddr_in saddr; 
    int saddr_len = sizeof(saddr);
    memset(&saddr, 0, sizeof(saddr));

    // Receive a network packet and copy into buffer
    unsigned char *rbuf = (unsigned char *) malloc(65536);
    if (recvfrom(rsfd, rbuf, 65536, 0, (struct sockaddr *) &saddr, (socklen_t *) &saddr_len) < 0) 
    {
        perror("recv FAILED");
        exit(EXIT_FAILURE);
    }

    // Printing the IP header received
    unsigned short iphrdlen;
    struct iphdr *ip = (struct iphdr *)(rbuf);
    printIPHeader(ip);
    memset(&source_addr, 0, sizeof(source_addr));
    source_addr.sin_addr.s_addr = ip->saddr;
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_addr.s_addr = ip->daddr;
    int iphdrlen = ip->ihl * 4;

    // Printing payload/data
    char *data = (char *)(rbuf + iphdrlen);
    cout << "Data received: " << data << "\n";

    return 0;
}