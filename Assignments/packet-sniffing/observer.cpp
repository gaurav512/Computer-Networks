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

int tcp = 0, udp = 0, others = 0, total = 0;

void displayIPHeader(char *buffer, int size)
{
	unsigned short iphdrlen;
	struct sockaddr_in source, dest;

	struct iphdr *iph = (struct iphdr *)buffer;
	iphdrlen = iph->ihl * 4;

	memset(&source, 0, sizeof(source));
	source.sin_addr.s_addr = iph->saddr;

	memset(&dest, 0, sizeof(dest));
	dest.sin_addr.s_addr = iph->daddr;

	cout << "\n";
	cout << "****IP Header****\n";
	cout << "Protocol: " << (unsigned int)iph->protocol << "\t";
	cout << "Source IP: " << inet_ntoa(source.sin_addr) << "\t";
	cout << "Destination IP: " << inet_ntoa(dest.sin_addr) << "\t";
	cout << "Checksum: " << ntohs(iph->check) << "\n";
}

void displayTCP(char *buffer, int size)
{
	unsigned short iphdrlen;

	struct iphdr *iph = (struct iphdr *)buffer;
	iphdrlen = iph->ihl * 4;

	struct tcphdr *tcph = (struct tcphdr *)(buffer + iphdrlen);

	cout << "```TCP PACKET```\n";

	displayIPHeader(buffer, size);

	cout << "\n";
	cout << "TCP Header\n";
	cout << "Source Port: " << ntohs(tcph->source) << "\t";
	cout << "Destination Port: " << ntohs(tcph->dest) << "\n";
}

void displayUDP(char *buffer, int size)
{

	unsigned short iphdrlen;

	struct iphdr *iph = (struct iphdr *)buffer;
	iphdrlen = iph->ihl * 4;

	struct udphdr *udph = (struct udphdr *)(buffer + iphdrlen);

	cout << "```UDP PACKET```\n";

	displayIPHeader(buffer, size);

	cout << "\nUDP Header\n";
	cout << "Source Port: " << ntohs(udph->source) << "\t";
	cout << "Destination Port: " << ntohs(udph->dest) << "\t";
	cout << "UDP Length: " << ntohs(udph->len) << "\t";
	cout << "UDP Checksum: " << ntohs(udph->check) << "\n";
}

int main()
{
	int saddr_size, data_size, rsfd;
	struct sockaddr saddr;
	struct in_addr in;

	char *buffer = (char *)malloc(65536);
	if ((rsfd = socket(AF_INET, SOCK_RAW, IPPROTO_TCP)) < 0)
	{
		perror("socket FAILED");
		exit(EXIT_FAILURE);
	}

	while (1)
	{
		saddr_size = sizeof(saddr);
		data_size = recvfrom(rsfd, buffer, 65536, 0, &saddr, (socklen_t *)&saddr_size);
		if (data_size < 0)
		{
			perror("recv FAILED");
			exit(EXIT_FAILURE);
		}
		struct iphdr *iph = (struct iphdr *)buffer;
		total++;

		if(iph->protocol == 6) {
			tcp++;
			displayTCP(buffer, data_size);
		}
		else if(iph->protocol == 17) {
			udp++;
			displayUDP(buffer, data_size);
		}
		else {
			others++;
		}
		printf("TCP : %d\tUDP : %d\tOthers : %d\tTotal : %d\n", tcp, udp, others, total);
	}

	close(rsfd);
	return 0;
}
