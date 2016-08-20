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
#include <signal.h>
#include <sys/un.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <bits/stdc++.h>
#include <dirent.h>
#include <grp.h>
#include <pwd.h>
#include <time.h>
#include <fcntl.h>
#include <math.h>

#define SA (struct sockaddr*)

using namespace std ;
vector <double> TIME ;

int packets_send = 0 , packet_received = 0 ; 
char global_ip[20] ;
struct timespec ping_start={0,0}, ping_end={0,0} ;

void stats_print(int a)
{
	clock_gettime(CLOCK_MONOTONIC, &ping_end);
	double percent = packets_send - packet_received ;
	percent = percent / packets_send ;
	percent *= 100 ;
	printf("\n--- %s ping statistics ---\n" , global_ip) ;
	if(packets_send == packet_received)
		printf("%d packets transmitted, %d received,  %.2f%% packet loss, time %d ms\n", packets_send , packet_received ,
																			   percent , (int)((((double)ping_end.tv_sec + 1.0e-9*ping_end.tv_nsec) - ((double)ping_start.tv_sec + 1.0e-9*ping_start.tv_nsec))*1000));
	else
		printf("%d packets transmitted, %d received, +%d errors, %.2f%% packet loss, time %d ms\n", packets_send , packet_received , packets_send - packet_received ,
																			   percent , (int)((((double)ping_end.tv_sec + 1.0e-9*ping_end.tv_nsec) - ((double)ping_start.tv_sec + 1.0e-9*ping_start.tv_nsec))*1000));

	double max_rtt = 0 , min_rtt = 100000000 , avg_rtt = 0 , mdev = 0 , total = 0  , mdev_total = 0 ;

	if(!TIME.empty())
	{
		int l = TIME.size() ;
		for(int i = 0 ; i < l ; i++)
		{
			total += TIME[i] ;
			mdev_total += (TIME[i]*TIME[i]) ;
			if(TIME[i] > max_rtt)
				max_rtt = TIME[i] ;
			if(TIME[i] < min_rtt)
				min_rtt = TIME[i] ;
		}
		avg_rtt = total / l ;
		mdev = sqrt(mdev_total/l -((total/l)*(total/l))) ;
		printf("rtt min/avg/max/mdev = %.3f/%.3f/%.3f/%.3f m\n" , min_rtt , avg_rtt , max_rtt ,mdev ) ;
	}

	exit(1) ;

}

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
		strcpy(buff , inet_ntoa(*addr_list[i])) ;
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
	char buf[64] = { 0 };
	struct ip *ip_hdr = (struct ip *) buf;
	int hop = 64;

	int one = 1;
	const int *val = &one;
	if (setsockopt (sfd, IPPROTO_IP, IP_HDRINCL, val, sizeof (one)) < 0)
		printf ("Cannot set HDRINCL!\n");

	struct sockaddr_in addr;
	addr.sin_port = htons (7);
	addr.sin_family = AF_INET;
	printf("PING %s (",argv[1]) ;
	//if((argv[1][0] >='a' && argv[1][0] <='z')||(argv[1][0] >='A' && argv[1][0] <='Z') || argv[1][0] == '_')
	//	 get_IPP(argv[1]) ;
    

    
	strcpy(global_ip , argv[1]) ;
	if((global_ip[0] >='a' && global_ip[0] <='z')||(global_ip[0] >='A' && global_ip[0] <='Z') || global_ip[0] == '_')
		get_IPP(global_ip) ;
	printf("%s) 56(84) bytes of data.\n",global_ip) ;
	inet_pton (AF_INET, argv[1], &(addr.sin_addr));
	struct timespec tstart={0,0}, tend={0,0} ;  
	//printf("traceroute to %s (%s), 64 hops max\n", argv[1] , argv[1]) ;
	int seq_num = 0 ;

	signal(SIGINT, stats_print);
	// struct sigaction sighandle;
	// sighandle.sa_flags = SA_SIGINFO;
	// sighandle.sa_sigaction = &stats_print ;
	// sigaction(SIGINT , &sighandle, NULL);
	usleep(600) ;
	clock_gettime(CLOCK_MONOTONIC, &ping_start);
	while (1)
	{
		seq_num++ ;
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
		struct timespec *tstart1 = (struct timespec*) (buf+20+sizeof(struct icmphdr));
		clock_gettime(CLOCK_MONOTONIC, tstart1); 
		sendto (sfd, buf, sizeof(struct ip) + sizeof(struct icmphdr), 0, SA & addr, sizeof(addr));
		packets_send++ ;
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
		packet_received++ ;
		clock_gettime(CLOCK_MONOTONIC, &tend);
		struct icmphdr *icmphd2 = (struct icmphdr *) (buff + 20);
		printf("64 bytes from %s: icmp_seq=%d ttl=64 time=%.3f ms\n" , inet_ntoa(addr2.sin_addr) , seq_num , (((double)tend.tv_sec + 1.0e-9*tend.tv_nsec) - ((double)tstart.tv_sec + 1.0e-9*tstart.tv_nsec))*1000) ;
		TIME.push_back((((double)tend.tv_sec + 1.0e-9*tend.tv_nsec) - ((double)tstart.tv_sec + 1.0e-9*tstart.tv_nsec))*1000) ;
		sleep(1) ;        


	}
	return 0 ;
}