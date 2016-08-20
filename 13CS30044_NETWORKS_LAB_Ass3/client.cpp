#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <bits/stdc++.h>
#include <unistd.h>
 #include <poll.h>
#include <sys/socket.h>

using namespace std ;

struct rtlpheader
{
  unsigned short int src_port ;
  unsigned short int dst_port ;
  unsigned int seq_num  ; 
  unsigned int fin_num  ;
  unsigned int ack_num  ;
  unsigned int check_sum;
  char data[600] ;

  unsigned int fin:1      ;
  unsigned int ack:1      ;
  unsigned int syn:1      ;
  unsigned int syn_ack:1  ;
  unsigned int data_send:1;

} ;

unsigned  int  csum(struct rtlpheader *r )
{ 
    unsigned int ret = r->src_port + r->dst_port ;
    ret += r->seq_num ;
    ret =  (ret>>2) + r->ack_num ;
    ret +=  *((unsigned int *)r->data) ;

    return ret ;

}

int main(int argc , char * argv[]) 
{
	int s , n , check ; char buffer[1050] , buffer2[1050] ;
	struct sockaddr_in src , dst ;
    memset(buffer,0,sizeof(buffer)) ;

    socklen_t len = sizeof(dst);
    int fromlen = sizeof(dst);


    struct iphdr *ip = (struct iphdr *)buffer ;
    struct rtlpheader *rtlp = (struct rtlpheader *)(buffer + sizeof(struct iphdr)) ;
    struct rtlpheader *rtlp2 = (struct rtlpheader*)(buffer2 + sizeof(struct iphdr)); 

    if((s = socket(AF_INET , SOCK_RAW , IPPROTO_RAW)) < 0)
     { 
  	   perror("socket() error") ;
  	   exit(1) ;
     }   
  

  // Explicitly initializing the variables of the struct sockaddr

   src.sin_family = AF_INET ;
   dst.sin_family = AF_INET ;
   src.sin_port = 0 ;
   dst.sin_port = 0 ;
   src.sin_addr.s_addr = inet_addr(argv[1]) ;
   dst.sin_addr.s_addr = inet_addr(argv[2]) ;      
  
  // Initializing the variables of the IP-HEADER
   ip->ihl = 5;
   ip->version = 4;
   ip->tos = 0;
   ip->tot_len = htons(sizeof(buffer));	/* 16 byte value */
   ip->frag_off = 0;		/* no fragment */
   ip->ttl = 64;			/* default value */
   ip->protocol = IPPROTO_RAW;	/* protocol at L4 */
   ip->check = 0;			/* not needed in iphdr */
   ip->saddr = inet_addr(argv[1]);
   ip->daddr = inet_addr(argv[2]);

   rtlp->src_port = htons(atoi(argv[3])) ;
   rtlp->dst_port = htons(atoi(argv[4])) ;
   // preparing the syn packet
   rtlp->seq_num = 1224 ;
   rtlp->ack_num = 0 ; 
   rtlp->fin = 0 ;
   rtlp->ack = 0 ;
   rtlp->syn = 1 ;
   rtlp->syn_ack = 0 ;
   rtlp->data_send = 0 ;
   rtlp->check_sum = csum(rtlp) ;
   
   cout << "********************************************\n"; 
   cout << "          INITIATING CONNECTION              " << endl ;
   cout << "********************************************\n\n";

   cout << "Sending SEQ number (SYN packet) : " << rtlp->seq_num << endl ; 
   
   if(sendto(s , buffer, sizeof(buffer) , 0, (struct sockaddr *)&dst, sizeof(src)) < 0)
    { 
      perror("sendto() error");
      exit(-1);
    }

   fd_set rfds;
   struct timeval tv;
   int retval;

   /* Watch stdin (fd 0) to see when it has input. */
   FD_ZERO(&rfds);
   FD_SET(s, &rfds);
   tv.tv_sec  = 1;
   tv.tv_usec = 0 ;
   retval = select(s+1, &rfds , NULL, NULL, &tv);
   
   if(retval == -1)
   {
   	perror("select()") ;
   	 exit(1) ;
   } 
    
    int count = 1 ;
   while(retval < 0 && count < 4)
   {
        cout << "Sending SYN pack again 1 " << endl; 
        if(sendto(s , buffer, sizeof(buffer) , 0, (struct sockaddr *)&dst, sizeof(src)) < 0)
        { 
          perror("sendto() error");
          exit(-1);
        }
        retval = select(s+1, &rfds, NULL, NULL, &tv);
   
        if(retval == -1)
       {
     	 perror("select()") ;
   	     exit(1) ;
       } 
       if(FD_ISSET(s,&rfds))
       	break ;
       count++ ;
   }

   if(retval < 0)
   {
   	 cout << "Cannot establish a connection(retval) " << endl ;
   	 exit(1) ;
   }
   

    count = 0 ;
    
    memset(buffer2,0,sizeof(buffer2)) ;
    sleep(1) ;
    cout << "SYN packet has been sent successfully" << endl ;    

   if(recvfrom(s, (char *)&buffer2 , sizeof(buffer2) , 0 ,
      (struct sockaddr *)&src , &len) < 0 )
   	{
   		perror("error in recieving syn_Ack") ;
   		exit(1) ;
   	}
   //cout << " YO" << endl ;
  
  

  int count2 = 0 ;

  while(rtlp2->ack_num != 1224 && count2 < 4)
  {
  	 cout << "Wrong ack_num" << endl ;
  	 count = 0 ;	
     if(recvfrom(s, (char *)&buffer2 , sizeof(buffer2) , 0,
      (struct sockaddr *)&src , &len) < 0 )
   	{
   		perror("error in recieving syn_Ack") ;
   		exit(1) ;
   	}
    count2++ ;
  rtlp2 = (struct rtlpheader*)(buffer2+sizeof(struct iphdr)); 
  } 
  
      
  if(rtlp2->ack_num != 1224)
   {
   	 cout << "Error in ack_number " << endl ;
   	 exit(1) ;
   }
   
  cout << "\nSYN-ACK pack recieved\n" << endl ;
  sleep(1) ; 
  cout << "SEQ Number Recieved (SYN-ACK packet) : " << rtlp2->seq_num << endl ;

  check = csum(rtlp2) ;
  
  if(check == rtlp2->check_sum)
  {
    sleep(1); 
   cout << "... Checksum matches " << endl ;
  } 
    
  else
  {
    cout << " Checksum ka load 2" << endl ;
    exit(1) ;
  } 



   
   rtlp->ack_num = rtlp2->seq_num ;
   //rtlp->seq_num = 0 ; 
   rtlp->fin = 0 ;
   rtlp->ack = 1 ;
   rtlp->syn = 0 ;
   rtlp->syn_ack = 0 ;
   rtlp->data_send = 0 ;
   rtlp->check_sum = csum(rtlp) ;

   cout << "\nSending the final ACK packet to the server...\n" ;  
   sleep(1) ;
   if(sendto(s , buffer, sizeof(buffer) , 0, (struct sockaddr *)&dst, sizeof(src)) < 0)
    { 
      perror("sendto() error");
      exit(-1);
    }


   /* Watch stdin (fd 0) to see when it has input. */
   //FD_ZERO(&rfds);
   //FD_SET(s, &rfds);
   tv.tv_sec  = 1 ;
   tv.tv_usec = 0 ;
   retval = select(s+1, &rfds, NULL, NULL, &tv);
   
   if(retval == -1)
   {
   	 perror("select()") ;
   	 exit(1) ;
   } 
    
   count = 1 ;
   while(retval < 0 && count < 4)
   {
        cout << "Sending  last SYN pack again " << endl; 
        if(sendto(s , buffer, sizeof(buffer) , 0, (struct sockaddr *)&dst, sizeof(src)) < 0)
        { 
          perror("sendto() error");
          exit(-1);
        }
        retval = select(s+1, &rfds, NULL, NULL, &tv);
   
        if(retval == -1)
       {
     	 perror("select()") ;
   	     exit(1) ;
       } 
       count++ ;
   }

   if(retval < 0)
   {
   	 cout << "Cannot establiish a connection " << endl ;
   	 exit(1) ;
   }
   
  cout << "\nConnection has been established with the server\n" << endl ;
  cout << "********************************************\n\n";

  sleep(1) ;
  
  //cout <<"\n\n"<< endl ;
  //cout << "RLTP seq " << rtlp->seq_num << endl ;
  //cout << "RLTP ack " << rtlp->ack_num << endl ;
  //cout << "RLTP2 seq " << rtlp2->seq_num << endl ; 
  //cout << "RLTP2 ack " << rtlp2->ack_num << endl ;
  
  int r_seq   = rtlp->seq_num          ;
  int r_ack   = rtlp->ack_num          ; 
  int r2_seq  = rtlp2->seq_num         ;
  int r2_ack  = rtlp2->ack_num         ;
  
  cout << "********************************************\n";
  cout  << "            Testing Phase  "  << endl ;
  cout << "********************************************\n";
  cout << "\n\nEnter the value of N : " ;
  cin >> n ; 

  struct pollfd timeout ;
  timeout.fd = s ;



  for(int i = n ; i <=10 ; i++)
  {
      memset(rtlp->data , 0 , sizeof(rtlp->data) ) ;
      sprintf(rtlp->data,"ECHO REQ %d",i) ;
      rtlp->seq_num += strlen(rtlp->data) ;
      rtlp->fin = 0 ;
      rtlp->ack = 0 ;
      rtlp->syn = 0 ;
      rtlp->syn_ack = 0 ;
      rtlp->data_send = 1 ;
      rtlp->check_sum = csum(rtlp) ;
      r_seq   = rtlp->seq_num          ;
      r_ack   = rtlp->ack_num          ;

      cout << "Sending : " << rtlp->data  << " to the server " << endl ;
      sleep(1) ;  

       if(sendto(s , buffer, sizeof(buffer) , 0, (struct sockaddr *)&dst, sizeof(src)) < 0)
      { 
          perror("sendto() error");
          exit(-1);
      }
      /*

      cout << "YO" << endl ;
      retval = poll(&timeout , 1 , 1000) ;

      if(retval == -1 ) 
      {
        perror("TIMEOUT ME LOAD HAI RE") ;
        exit(1) ;
      }
      cout << "YO" << endl ;
      count =1 ;
      while(retval == 0 && count < 4)
      {
        if(sendto(s , buffer, sizeof(buffer) , 0, (struct sockaddr *)&dst, sizeof(src)) < 0)
      { 
          perror("sendto() error");
          exit(-1);
      }
      cout << "YO" << endl ;
      count++ ;
      retval = poll(&timeout , 1 , 15000) ;
      cout << "YO" << endl ;
      }
     */

      FD_ZERO(&rfds);
    FD_SET(s, &rfds);
    tv.tv_sec  = 1;
    tv.tv_usec = 0 ;
    retval = select(s+1, &rfds , NULL, NULL, &tv);
   
   if(retval == -1)
   {
    perror("select()") ;
     exit(1) ;
   } 
    
    int count = 1 ;
   while(retval < 0 && count < 4)
   {
        cout << "Sending  packet again 1 " << endl; 
        if(sendto(s , buffer, sizeof(buffer) , 0, (struct sockaddr *)&dst, sizeof(src)) < 0)
        { 
          perror("sendto() error");
          exit(-1);
        }
        retval = select(s+1, &rfds, NULL, NULL, &tv);
   
        if(retval == -1)
       {
       perror("select()") ;
         exit(1) ;
       } 
       if(FD_ISSET(s,&rfds))
        break ;
       count++ ;
   }

   if(retval < 0)
   {
     cout << "Cannot establish a connection(retval) " << endl ;
     exit(1) ;
   }
      
      if(recvfrom(s, (char *)&buffer2 , sizeof(buffer2) , 0 ,
      (struct sockaddr *)&src , &len) < 0 )
      {
      perror("error in recieving syn_Ack") ;
      exit(1) ;
      }

      if(rtlp2->ack != 1  || rtlp2->data_send != 1 || r_seq != rtlp2->ack_num)
      {
        perror("Wrong packet recieved") ;
        exit(1) ;
      }
     
     r2_seq  = rtlp2->seq_num  ;
     r2_ack  = rtlp2->ack_num  ;
     



  check = csum(rtlp2) ;
  
  if(check == rtlp2->check_sum)
    cout << "... Checksum matches " << endl ;
  else
  {
    cout << " Checksum ka load4 " << endl ;
    exit(1) ;
  }

      cout << "Receive : " << rtlp2->data << " from the server" << endl ;
      sleep(1) ;

  }


  cout << "\n********************************************\n" ;
  cout << "          Initiating Termination              \n" ;
  cout << "********************************************\n" ;
 
   sleep(1) ;
   rtlp->fin_num = 5555 ; 
   rtlp->ack_num = 0 ;
   rtlp->fin = 1 ;
   rtlp->ack = 0 ;
   rtlp->syn = 0 ;
   rtlp->syn_ack = 0 ;
   rtlp->data_send = 0 ;
   rtlp->check_sum = csum(rtlp) ;

     cout << "Sending FIN number (FIN packet) : " << rtlp->fin_num << endl ; 
   
   if(sendto(s , buffer, sizeof(buffer) , 0, (struct sockaddr *)&dst, sizeof(src)) < 0)
    { 
      perror("sendto() error");
      exit(-1);
    }

   /* Watch stdin (fd 0) to see when it has input. */
   FD_ZERO(&rfds);
   FD_SET(s, &rfds);
   tv.tv_sec  = 1;
   tv.tv_usec = 0 ;
   retval = select(s+1, &rfds , NULL, NULL, &tv);
   
   if(retval == -1)
   {
    perror("select()") ;
     exit(1) ;
   } 
    
  count = 1 ;
   while(retval < 0 && count < 4)
   {
        cout << "Sending FIN pack again 1 " << endl; 
        if(sendto(s , buffer, sizeof(buffer) , 0, (struct sockaddr *)&dst, sizeof(src)) < 0)
        { 
          perror("sendto() error");
          exit(-1);
        }
        retval = select(s+1, &rfds, NULL, NULL, &tv);
   
        if(retval == -1)
       {
       perror("select()") ;
         exit(1) ;
       } 
       if(FD_ISSET(s,&rfds))
        break ;
       count++ ;
   }

   if(retval < 0)
   {
     cout << "Retval fin load " << endl ;
     exit(1) ;
   }
   

    count = 0 ;
    
    //memset(buffer2,0,sizeof(buffer2)) ;
    cout << "FIN packet has been sent successfully" << endl ;    

   if(recvfrom(s, (char *)&buffer2 , sizeof(buffer2) , 0 ,
      (struct sockaddr *)&src , &len) < 0 )
    {
      perror("error in recieving syn_Ack") ;
      exit(1) ;
    }
   //cout << " YO" << endl ;
  
  

  count2 = 0 ;

  while((rtlp2->ack != 1 || rtlp2->fin  != 1 || rtlp2->ack != rtlp->fin ) && count2 < 4)
  {
     cout << "Wrong ack_flag" << endl ;
     count = 0 ;  
     if(recvfrom(s, (char *)&buffer2 , sizeof(buffer2) , 0,
      (struct sockaddr *)&src , &len) < 0 )
    {
      perror("error in recieving syn_Ack") ;
      exit(1) ;
    }
    count2++ ;
  rtlp2 = (struct rtlpheader*)(buffer2+sizeof(struct iphdr)); 
  } 
  
      
  if(rtlp2->ack != 1 || rtlp2->fin  != 1 || rtlp2->ack != rtlp->fin )
   {
     cout << "Error in ack_number " << endl ;
     exit(1) ;
   }

   cout << "\nFIN-ACK pack recieved\n" << endl ;
  sleep(1) ; 
  cout << "FIN Number Recieved (SYN-ACK packet) : " << rtlp2->fin_num << endl ;


  check = csum(rtlp2) ;
  
  if(check == rtlp2->check_sum)
    cout << "... Checksum matches " << endl ; 
  else
  {
    cout << " Checksum ka load35 " << endl ;
    exit(1) ;
  }

   

   rtlp->ack_num = rtlp2->fin_num ;
   //rtlp->seq_num = 0 ; 
   //rtlp->fin = 0 ;
   rtlp->ack = 1 ;
   rtlp->syn = 0 ;
   rtlp->syn_ack = 0 ;
   rtlp->data_send = 0 ;
   rtlp->check_sum = csum(rtlp) ;

   cout << "\nSending the final ACK packet to the server...\n" ;  
   sleep(1) ;

   if(sendto(s , buffer, sizeof(buffer) , 0, (struct sockaddr *)&dst, sizeof(src)) < 0)
    { 
      perror("sendto() error");
      exit(-1);
    }


   /* Watch stdin (fd 0) to see when it has input. */
   //FD_ZERO(&rfds);
   //FD_SET(s, &rfds);
   tv.tv_sec  = 1 ;
   tv.tv_usec = 0 ;
   retval = select(s+1, &rfds, NULL, NULL, &tv);
   
   if(retval == -1)
   {
     perror("select()") ;
     exit(1) ;
   } 
    
   count = 1 ;
   while(retval < 0 && count < 4)
   {
        cout << "Sending  last FIN pack again " << endl; 
        if(sendto(s , buffer, sizeof(buffer) , 0, (struct sockaddr *)&dst, sizeof(src)) < 0)
        { 
          perror("sendto() error");
          exit(-1);
        }
        retval = select(s+1, &rfds, NULL, NULL, &tv);
   
        if(retval == -1)
       {
       perror("select()") ;
         exit(1) ;
       } 
       count++ ;
   }

   if(retval < 0)
   {
     cout << "Cannot establiish a connection " << endl ;
     exit(1) ;
   }
   
  cout << "Connection from the server has been terminated " << endl ; 
  cout << "********************************************\n" ;
  sleep(1) ;

}
