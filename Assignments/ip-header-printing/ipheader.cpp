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

int main()
{
    // Creating Raw socket
    int rsfd;
    if((rsfd = socket(AF_INET, SOCK_RAW, IPPROTO_TCP)) < 0)
    {
        perror("socket FAILED");
        exit(EXIT_FAILURE);
    }

    // Receive a raw packet
    char *buf = (char *) malloc(65536);
    memset(buf, 0, 65536);
    struct sockaddr addr;
    int addr_len = sizeof(struct sockaddr);

    while(true)
    {
        int retval = recvfrom(rsfd, buf, 65536, 0, &addr, (socklen_t *) &addr_len);
        if(retval < 0)
        {
            perror("recvfrom FAILED");
            exit(EXIT_FAILURE);
        }

        // Setting the pointer of ip header to the buffer
        unsigned short iphdrlen;
        struct iphdr *ip = (struct iphdr *) buf;

        // Setting the source and destination ip addresses obtained from *ip
        struct sockaddr_in source_addr, dest_addr;
        
        memset(&source_addr, 0, sizeof(sockaddr_in));
        source_addr.sin_addr.s_addr = ip->saddr;

        memset(&dest_addr, 0, sizeof(sockaddr_in));
        dest_addr.sin_addr.s_addr = ip->daddr;

        // Printing the ip header
        cout << "*** IP Header ***\n";
        cout << "   IP Version: " << ip->version << "\n";
        cout << "   IP Header Length: " << (unsigned int) ip->ihl << " bit words, or " << (unsigned int)ip->ihl*4 << " bytes\n";
        cout << "   Type of service: " << (unsigned int) ip->tos << "\n";
        cout << "   Total Packet Length: " << ntohs(ip->tot_len) << " bytes\n";
        cout << "   Packet ID Number: " << ntohs(ip->id) << "\n";
        cout << "   Time to Live(ttl) : " << (unsigned int) ip->ttl << "\n";
        cout << "   Protocol: " << (unsigned int) ip->protocol << "\n";
        cout << "   Header Checksum: " << ntohs(ip->check) << "\n";
        cout << "   Source IP Address: " << inet_ntoa(source_addr.sin_addr) << "\n";
        cout << "   Destination IP Address: " << inet_ntoa(dest_addr.sin_addr) << "\n\n"; 

    }

    return 0;
}