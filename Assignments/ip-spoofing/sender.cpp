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

    // Setting the datagram and data pointer
    char datagram[65536], source_ip[32] , *data , *pseudogram;
	memset(datagram, 0, 65536);
	
    struct iphdr *iph = (struct iphdr *) datagram;
	data = datagram + sizeof(struct iphdr);
	strcpy(data , "Hello darkness!");
	
    struct sockaddr saddr;

    // Fill the ip header
    iph->ihl = 5;
	iph->version = 4;
	iph->tos = 0;
	iph->tot_len = sizeof(struct iphdr) + strlen(data);
	iph->id = htonl (54321);	//Id of this packet
	iph->frag_off = 0;
	iph->ttl = 255;
	iph->protocol = 69;
	iph->check = 0;		//Set to 0 before calculating checksum
	iph->saddr = inet_addr ("169.69.69.169");	//Spoof the source ip address
	iph->daddr = inet_addr ("127.0.0.2");

	iph->check = csum((unsigned short *) datagram, (iph->ihl)*4);

    // setsockopt
    int opt = 1;
	const int *val = &opt;
	if (setsockopt(rsfd, IPPROTO_IP, IP_HDRINCL, val, sizeof(opt)) < 0) {
		perror("setsockopt FAILED");
		exit(EXIT_FAILURE);
	}	
	
    // Sending the datagram packet we created
    if (sendto(rsfd, datagram, iph->tot_len, 0, (struct sockaddr *) &saddr, sizeof(saddr)) < 0) {
		perror("sendto FAILED");
		exit(EXIT_FAILURE);
	}
	else 
	{
		cout << "Packet has been sent successfully\n";
	}

	return 0;

}