#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <time.h>
#include <sys/ipc.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <pthread.h>
#include <sys/select.h>
#include <sys/un.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <bits/stdc++.h>

#define SA (struct sockaddr*)

using namespace std ;

void get_IPP(char *buff)
{

    int i;
    struct hostent *he;
    struct in_addr **addr_list;


    if ((he = gethostbyname(buff)) == NULL) {  // get the host info
        herror("gethostbyname");
       // return 2;
    }

    // print information about this host:
    //printf("Official name is: %s\n", he->h_name);
    //printf("    IP addresses: ");
    addr_list = (struct in_addr **)he->h_addr_list;
    for(i = 0; addr_list[i] != NULL; i++) {
        //printf("%s ", inet_ntoa(*addr_list[i]));
    	strcpy(inet_ntoa(*addr_list[i]) , buff) ;
    }
    //printf("\n");
}

unsigned short
csum (unsigned short *buf, int nwords)
{
  unsigned long sum;
  for (sum = 0; nwords > 0; nwords--)
	sum += *buf++;
  sum = (sum >> 16) + (sum & 0xffff);
  sum += (sum >> 16);
  return ~sum;
}

void get_ip(char * ip)
{
	int n;
	struct ifreq ifr;
	char array[] = "eth0";
 
	n = socket(AF_INET, SOCK_DGRAM, 0);
	//Type of address to retrieve - IPv4 IP address
	ifr.ifr_addr.sa_family = AF_INET;
	//Copy the interface name in the ifreq structure
	strncpy(ifr.ifr_name , array , IFNAMSIZ - 1);
	ioctl(n, SIOCGIFADDR, &ifr);
	close(n);
	//display result
	strcpy( ip , inet_ntoa(( (struct sockaddr_in *)&ifr.ifr_addr )->sin_addr) );
}

int main (int argc, char *argv[])
{
	if (argc != 2)
	{
		printf ("need destination for tracert\n");
		exit (0);
	}
	int sfd = socket (AF_INET, SOCK_RAW, IPPROTO_ICMP);
	char buf[4096] = { 0 };
	struct ip *ip_hdr = (struct ip *) buf;
	int hop = 1;

	int one = 1;
	const int *val = &one;
	if (setsockopt (sfd, IPPROTO_IP, IP_HDRINCL, val, sizeof (one)) < 0)
		printf ("Cannot set HDRINCL!\n");

	struct sockaddr_in addr;
	addr.sin_port = htons (7);
	addr.sin_family = AF_INET;
	if((argv[1][0] >='a' && argv[1][0] <='z')||(argv[1][0] >='A' && argv[1][0] <='Z'))
		 get_IPP(argv[1]) ;

	inet_pton (AF_INET, argv[1], &(addr.sin_addr));
	struct timespec tstart={0,0}, tend={0,0} ; 	
	printf("traceroute to %s (%s), 64 hops max\n", argv[1] , argv[1]) ;

	while (1)
	{
		ip_hdr->ip_hl = 5;
		ip_hdr->ip_v = 4;
		ip_hdr->ip_tos = 0;
		ip_hdr->ip_len = 20 + 8;
		ip_hdr->ip_id = 10000;
		ip_hdr->ip_off = 0;
		ip_hdr->ip_ttl = hop;
		ip_hdr->ip_p = IPPROTO_ICMP;
		char ip[20] ;
		get_ip(ip) ;
		//cout << ip << endl;
		inet_pton (AF_INET, ip , &(ip_hdr->ip_src));
		inet_pton (AF_INET, argv[1], &(ip_hdr->ip_dst));	
		ip_hdr->ip_sum = csum ((unsigned short *) buf, 9);

		struct icmphdr *icmphd = (struct icmphdr *) (buf + 20);
		icmphd->type = ICMP_ECHO;
		icmphd->code = 0;
		icmphd->checksum = 0;
		icmphd->un.echo.id = 0;
		icmphd->un.echo.sequence = hop + 1;
		icmphd->checksum = csum ((unsigned short *) (buf + 20), 4);
		clock_gettime(CLOCK_MONOTONIC, &tstart);
		sendto (sfd, buf, sizeof(struct ip) + sizeof(struct icmphdr), 0, SA & addr, sizeof addr);
		char buff[4096] = { 0 };
		struct sockaddr_in addr2;
        
		fd_set rfds;
   		struct timeval tv;
   		int result;

        FD_ZERO(&rfds);
   		FD_SET(sfd , &rfds);
   		tv.tv_sec  = 5 ;
   		tv.tv_usec = 0 ;
   		result = select(sfd+1, &rfds, NULL, NULL, &tv);   
  		if(result == -1)
   		{
     		perror("select()") ;
     		exit(1) ;
   		}

   		if(FD_ISSET(sfd, &rfds))
   		{
   			// skip this part
   		}
   		else
   		{
   			printf("Destination IP address is incorrect\n") ;
   			exit(1) ;
   		}	 

		socklen_t len = sizeof (struct sockaddr_in);
		recvfrom (sfd, buff, sizeof(buff), 0, SA & addr2, &len);
		clock_gettime(CLOCK_MONOTONIC, &tend);
		struct icmphdr *icmphd2 = (struct icmphdr *) (buff + 20);
		printf (" %d   %s", hop, inet_ntoa(addr2.sin_addr));
		printf("   %.3fms",(((double)tend.tv_sec + 1.0e-9*tend.tv_nsec) - ((double)tstart.tv_sec + 1.0e-9*tstart.tv_nsec))*1000);	

		ip_hdr->ip_hl = 5;
		ip_hdr->ip_v = 4;
		ip_hdr->ip_tos = 0;
		ip_hdr->ip_len = 20 + 8;
		ip_hdr->ip_id = 10000;
		ip_hdr->ip_off = 0;
		ip_hdr->ip_ttl = hop;
		ip_hdr->ip_p = IPPROTO_ICMP;
		inet_pton (AF_INET, ip , &(ip_hdr->ip_src));
		inet_pton (AF_INET, argv[1], &(ip_hdr->ip_dst));	
		ip_hdr->ip_sum = csum ((unsigned short *) buf, 9);

		icmphd = (struct icmphdr *) (buf + 20);
		icmphd->type = ICMP_ECHO;
		icmphd->code = 0;
		icmphd->checksum = 0;
		icmphd->un.echo.id = 0;
		icmphd->un.echo.sequence = hop + 1;
		icmphd->checksum = csum ((unsigned short *) (buf + 20), 4);
		clock_gettime(CLOCK_MONOTONIC, &tstart);
		sendto (sfd, buf, sizeof(struct ip) + sizeof(struct icmphdr), 0, SA & addr, sizeof addr);
		//char buff[4096] = { 0 };
		memset(buff , 0 , sizeof(buff)) ;
		//struct sockaddr_in addr2;
		len = sizeof (struct sockaddr_in);
		recvfrom (sfd, buff, sizeof(buff), 0, SA & addr2, &len);
		clock_gettime(CLOCK_MONOTONIC, &tend);
		icmphd2 = (struct icmphdr *) (buff + 20);
		//printf ("hop limit:%d Address:%s", hop, inet_ntoa (addr2.sin_addr));
		printf("  %.3fms",(((double)tend.tv_sec + 1.0e-9*tend.tv_nsec) - ((double)tstart.tv_sec + 1.0e-9*tstart.tv_nsec))*1000);

		ip_hdr->ip_hl = 5;
		ip_hdr->ip_v = 4;
		ip_hdr->ip_tos = 0;
		ip_hdr->ip_len = 20 + 8;
		ip_hdr->ip_id = 10000;
		ip_hdr->ip_off = 0;
		ip_hdr->ip_ttl = hop;
		ip_hdr->ip_p = IPPROTO_ICMP;
		inet_pton (AF_INET, ip , &(ip_hdr->ip_src));
		inet_pton (AF_INET, argv[1], &(ip_hdr->ip_dst));	
		ip_hdr->ip_sum = csum ((unsigned short *) buf, 9);

		icmphd = (struct icmphdr *) (buf + 20);
		icmphd->type = ICMP_ECHO;
		icmphd->code = 0;
		icmphd->checksum = 0;
		icmphd->un.echo.id = 0;
		icmphd->un.echo.sequence = hop + 1;
		icmphd->checksum = csum ((unsigned short *) (buf + 20), 4);
		clock_gettime(CLOCK_MONOTONIC, &tstart);
		sendto (sfd, buf, sizeof(struct ip) + sizeof(struct icmphdr), 0, SA & addr, sizeof addr);
		//char buff[4096] = { 0 };
		memset(buff , 0 , sizeof(buff)) ;
		//struct sockaddr_in addr2;
		len = sizeof (struct sockaddr_in);
		recvfrom (sfd, buff, sizeof(buff), 0, SA & addr2, &len);
		clock_gettime(CLOCK_MONOTONIC, &tend);
		icmphd2 = (struct icmphdr *) (buff + 20);
		//printf ("hop limit:%d Address:%s", hop, inet_ntoa (addr2.sin_addr));
		printf("  %.3fms\n",(((double)tend.tv_sec + 1.0e-9*tend.tv_nsec) - ((double)tstart.tv_sec + 1.0e-9*tstart.tv_nsec))*1000);
		if (icmphd2->type == 0 || hop == 64)
			break ;
		if(!strcmp(inet_ntoa (addr2.sin_addr) , ip)) exit(1) ;

		hop++;
	}
	if(hop == 64)
		cout << "Hop limit exceeded" << endl ;

	return 0;
}