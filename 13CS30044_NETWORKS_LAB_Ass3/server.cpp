#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <bits/stdc++.h>
#include <unistd.h>
#include <sys/socket.h>

using namespace std ;

struct rtlpheader
{
  unsigned short int src_port ;
  unsigned short int dst_port ;
  unsigned int seq_num  ; 
  unsigned int fin_num ;
  unsigned int ack_num  ;
  unsigned int check_sum;
  char data[600];

  unsigned int fin:1 ;
  unsigned int ack:1  ;
  unsigned int syn:1  ;
  unsigned int syn_ack:1 ;
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
  int s , check  ; char buffer[1050],buffer2[1050] ;
  struct sockaddr_in src , dst ;
  unsigned int client_seq_num;
  unsigned int client_ack_num;
  unsigned int server_seq_num;
  unsigned int server_ack_num;
    memset(buffer,0,sizeof(buffer)) ;
    memset(buffer2,0,sizeof(buffer2));

    socklen_t len = sizeof(dst);
    int fromlen = sizeof(dst);


    struct iphdr *ip = (struct iphdr *)buffer2 ;
    struct rtlpheader *rtlp = (struct rtlpheader *)(buffer+ sizeof(struct iphdr)) ;
    struct rtlpheader *rtlp2 = (struct rtlpheader *)(buffer2+ sizeof(struct iphdr));
    if((s = socket(AF_INET , SOCK_RAW , IPPROTO_RAW)) < 0)
     { 
       perror("socket() error") ;
       exit(1) ;
     }   

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
   ip->tot_len = htons(sizeof(buffer2));  /* 16 byte value */
   ip->frag_off = 0;    /* no fragment */
   ip->ttl = 64;      /* default value */
   ip->protocol = IPPROTO_RAW;  /* protocol at L4 */
   ip->check = 0;     /* not needed in iphdr */
   ip->saddr = inet_addr(argv[1]);
   ip->daddr = inet_addr(argv[2]);
    

  memset(buffer,0,sizeof(buffer)) ;
  cout << "****************************************\nWaiting for Initial Connection Request\n****************************************\n\n" << endl ;


    if(recvfrom(s, (char *)&buffer, sizeof(buffer), 0,
      (struct sockaddr *)&src , &len) < 0 )
    {
      perror("error in recieving syn packet") ;
      exit(1) ;
    }
   
    //cout << "Just received" << endl ;
  
 // cout << "Seq number recieved : " << rtlp->seq_num << endl ;
   //cout << "Seq number being sent : 3342" << endl ;
  //cout << rtlp->syn<< endl;
  int count2 = 0 ;

  while(rtlp->syn != 1 && count2 < 4)
  {
     cout << "Wrong syn_num" << endl ;
     count2 = 0 ;
     if(recvfrom(s, (char *)&buffer, sizeof(buffer), 0,
      (struct sockaddr *)&src , &len) < 0 )
    {
      perror("error in recieving syn_Ack") ;
      exit(1) ;
    }
    count2++ ;
  rtlp = (struct rtlpheader*)(buffer+sizeof(struct iphdr)); 
  } 
  
      
  if(rtlp->syn != 1)
   {
     cout << "Error in ack_number " << endl ;
     exit(1) ;
   } 

   rtlp = (struct rtlpheader*)(buffer+sizeof(struct iphdr)); 

  check = csum(rtlp) ;
  
  if(check == rtlp->check_sum)
    cout << "\n---Received Request---\n\n---Checksum For First Request Matched---\n" << endl ; 
  else
  {
    cout << "\n\n---Checksum Mismatch---\n" << endl ;
    exit(1) ;
  } 


   
   cout << "Client Sequence number Recieved : " << rtlp->seq_num << endl ;
   cout << "Server Sequence number Sent : 3342" << endl ;
   rtlp2->ack_num = rtlp->seq_num ;
   rtlp2->seq_num = 3342 ; 
   rtlp2->fin = 0 ;
   rtlp2->ack = 0 ;
   rtlp2->syn = 0 ;
   rtlp2->syn_ack = 1 ;
   rtlp2->check_sum = csum(rtlp2) ;

   if(sendto(s , buffer2, sizeof(buffer2) , 0, (struct sockaddr *)&dst, sizeof(src)) < 0)
    { 
      perror("sendto() error");
      exit(-1);
    }
  cout << "\n\n---Sending SYN_ACK pack---\n" << endl; 
        
   fd_set rfds;
   struct timeval tv;
   int retval;

   /* Watch stdin (fd 0) to see when it has input. */
   FD_ZERO(&rfds);
   FD_SET(s, &rfds);
   tv.tv_sec  = 1 ;
   tv.tv_usec = 0 ;
   retval = select(s+100, &rfds, NULL, NULL, &tv);
   
   if(retval == -1)
   {
     perror("select()") ;
     exit(1) ;
   } 
    
    int count = 1 ;
   while(retval <0 && count < 4)
   {
        cout << "\n\n---Sending SYN_ACK pack again---\n" << endl; 
        if(sendto(s , buffer2, sizeof(buffer2) , 0, (struct sockaddr *)&dst, sizeof(src)) < 0)
        { 
          perror("sendto() error in syn_ack ");
          exit(-1);
        }
        retval = select(s+100, &rfds, NULL, NULL, &tv);
   
        if(retval == -1)
       {
       perror("select()") ;
         exit(1) ;
       } 
       count++ ;
   }

   if(retval < 0)
   {
     cout << "Cannot establish a connection(retval) " << endl ;
     exit(1) ;
   }
   //cout << "Waiting for Receving  2 "<< endl;
   
   if(recvfrom(s, (char *)&buffer, sizeof(buffer), 0,
      (struct sockaddr *)&src , &len) < 0 )
    {
      perror("error in recieving syn_Ack") ;
      exit(1) ;
    }
   //cout << "Just Received 2 " << endl;
  
  rtlp = (struct rtlpheader*)(buffer+sizeof(struct iphdr)); 

   count2 = 0 ;

  while(rtlp->ack_num != 3342 && count2 < 4)
  {
     cout << "Wrong ack_num last" << endl ;
     count = 0 ;
     if(recvfrom(s, (char *)&buffer, sizeof(buffer), 0,
      (struct sockaddr *)&src , &len) < 0 )
    {
      perror("error in recieving Ack") ;
      exit(1) ;
    }
    count2++ ;
  rtlp = (struct rtlpheader*)(buffer+sizeof(struct iphdr)); 
  } 
  
      
  if(rtlp->ack_num != 3342)
   {
     cout << "Error in ack_number " << endl ;
     exit(1) ;
   }

  rtlp = (struct rtlpheader*)(buffer+sizeof(struct iphdr)); 

  check = csum(rtlp) ;
  
  if(check == rtlp->check_sum)
    cout << "\n\n---Checksum For Final Acknowledgement Matched---\n\n" << endl ; 
  else
  {
    cout << "\n\n---Checksum Mismatch---\n\n" << endl ;
    exit(1) ;
  } 
  sleep(2); 
   cout << "Client Initial Acknowledgement Number : " << rtlp->ack_num << endl ;
   cout << "Client Initial Sequence Number : " << rtlp->seq_num << endl;
   cout << "Server Initial Acknowledgement Number : " << rtlp2->ack_num << endl ;
   cout << "Server Initial Sequence Number : " << rtlp2->seq_num << endl;
   
   cout << "*************************************\nConnection Established\n*************************************"<< endl ;

  client_seq_num = rtlp->seq_num;
  client_ack_num = rtlp->ack_num;
  server_seq_num = rtlp2->seq_num;
  server_ack_num = rtlp2->ack_num;

   while(1)
   {

      cout << "---Waiting for Client Data---"<< endl;
   
      if(recvfrom(s, (char *)&buffer, sizeof(buffer), 0,
        (struct sockaddr *)&src , &len) < 0 )
      {
        perror("error in recieving syn_Ack") ;
        exit(1) ;
      }
      //cout << "Just Received 3 " << endl;
  
      rtlp = (struct rtlpheader*)(buffer+sizeof(struct iphdr)); 

      count2 = 0 ;
      //cout << rtlp->fin << endl;
      //cout <<  rtlp->ack_num << endl;
  while(!((rtlp->fin == 1) ^ (rtlp->ack_num == client_ack_num)) && count2 < 4)
  {
     cout << "Wrong num in while(1)" << endl ;
     count2 = 0 ;
     if(recvfrom(s, (char *)&buffer, sizeof(buffer), 0,
      (struct sockaddr *)&src , &len) < 0 )
    {
      perror("error in recieving syn_Ack") ;
      exit(1) ;
    }
    count2++ ;
  rtlp = (struct rtlpheader*)(buffer+sizeof(struct iphdr)); 
  } 
  
      
  if(!((rtlp->fin == 1) ^ (rtlp->ack_num == client_ack_num)))
   {
     cout << "Error in while(1) loop " << endl ;
     exit(1) ;
   } 


      if(rtlp->fin == 1)
      {



       check = csum(rtlp) ;
  
        if(check == rtlp->check_sum)
         cout << "****************************************\nReceived Request to Terminate\n****************************************\n---Checksum For Initial Termination Request Matched---\n" << endl ; 
         else
         {
           cout << "---Checksum Mismatch" << endl ;
             exit(1) ;
         }
   
        cout << "\n\nClient Final number Recieved : " << rtlp->fin_num << endl ;
        cout << "Server Final number Send : 7342\n\n" << endl ;
         rtlp2->ack_num = rtlp->fin_num ;
         rtlp2->fin_num = 7342 ; 
         rtlp2->fin = 1 ;
         rtlp2->ack = 1 ;
         rtlp2->syn = 0 ;
         rtlp2->syn_ack = 0 ;
         rtlp2->check_sum = csum(rtlp2) ;

           if(sendto(s , buffer2, sizeof(buffer2) , 0, (struct sockaddr *)&dst, sizeof(src)) < 0)
            { 
              perror("sendto() error");
              exit(-1);
            }
          cout << "---Sending SYN_ACK pack---" << endl; 
                
           fd_set rfds;
           struct timeval tv;
           int retval;

           /* Watch stdin (fd 0) to see when it has input. */
           FD_ZERO(&rfds);
           FD_SET(s, &rfds);
           tv.tv_sec  = 1 ;
           tv.tv_usec = 0 ;
           retval = select(s+1, &rfds, NULL, NULL, &tv);
           
           if(retval == -1)
           {
             perror("select()") ;
             exit(1) ;
           } 
            
            int count = 1 ;
           while(retval <0 && count < 4)
           {
                cout << "Sending SYN_ACK pack again " << endl; 
                if(sendto(s , buffer2, sizeof(buffer2) , 0, (struct sockaddr *)&dst, sizeof(src)) < 0)
                { 
                  perror("sendto() error in syn_ack ");
                  exit(-1);
                }
                retval = select(s+100, &rfds, NULL, NULL, &tv);
           
                if(retval == -1)
               {
               perror("select()") ;
                 exit(1) ;
               } 
               count++ ;
           }

           if(retval < 0)
           {
             cout << "Cannot establiish a connection(retval) " << endl ;
             exit(1) ;
           }
           //cout << "Waiting for Receving  2 "<< endl;
           
           if(recvfrom(s, (char *)&buffer, sizeof(buffer), 0,
              (struct sockaddr *)&src , &len) < 0 )
            {
              perror("error in recieving syn_Ack") ;
              exit(1) ;
            }
           //cout << "Just Received 2 " << endl;
          
          rtlp = (struct rtlpheader*)(buffer+sizeof(struct iphdr)); 

           count2 = 0 ;

          while(rtlp->ack_num != 7342 && count2 < 4)
          {
             cout << "Wrong ack_num last" << endl ;
             count = 0 ;
             if(recvfrom(s, (char *)&buffer, sizeof(buffer), 0,
              (struct sockaddr *)&src , &len) < 0 )
            {
              perror("error in recieving Ack") ;
              exit(1) ;
            }
            count2++ ;
          rtlp = (struct rtlpheader*)(buffer+sizeof(struct iphdr)); 
          } 
          
              
          if(rtlp->ack_num != 7342)
           {
             cout << "Error in ack_number " << endl ;
             exit(1) ;
           }




       check = csum(rtlp) ;
  
       if(check == rtlp->check_sum)
       cout << "---Checksum For Final Acknowledgement Matched---" << endl ; 
      else
      {
        cout << "---Checksum Mismatch" << endl ;
        exit(1) ;
      } 
          sleep(2); 
           cout << "\n\nClient Final Acknowledgement Number : " << rtlp->ack_num << endl ;
           cout << "Client Final Sequence Number : " << rtlp->fin_num << endl;
           cout << "Server Final Acknowledgement Number : " << rtlp2->ack_num << endl ;
           cout << "Server Final Sequence Number : " << rtlp2->fin_num << endl;

           cout << "\n\n**********************************Connection Terminated**********************************\n\n"<< endl ;
           break;
      }


  check = csum(rtlp) ;
  
  if(check == rtlp->check_sum)
    cout << "---Checksum Matched---" << endl ; 
  else
  {
    cout << "---Checksum Mismatch---" << endl ;
    exit(1) ;
  }
      sleep(2);

      if(rtlp->data_send == 1)
      {
        cout << strlen(rtlp->data) << endl;
        if(strlen(rtlp->data) == rtlp->seq_num- client_seq_num)
        {
          cout << "---Size received is Correct---" << endl;
        }
        char *teststring ;
        teststring = rtlp->data;
        cout <<"\n\n\nMessage Received : "<< teststring << endl;
        char *test2string ;
        char test2[10];
        test2string = teststring+9; 
        //cout << test2string << endl;
        strcpy(test2,test2string);
        //cout << test2 <<endl;
        int aforchanging = atoi(test2);
        //cout << aforchanging << endl;

        char test3[50];
        strcpy(test3,"ECHO RES ");
        //cout << test3 << endl;
        char intnum[50];
        sprintf(intnum,"%d",aforchanging+1);
        //cout << intnum << endl;
        strcat(test3,intnum);
        //cout <<"Message Sent : " << test3 << endl;
        //cout << strlen(test3) << endl;

        strcpy(rtlp2->data,test3);
        cout <<"Message Sent : "<< rtlp2->data << endl;
        rtlp2->ack_num = rtlp->seq_num ;
        rtlp2->seq_num = server_seq_num + strlen(test3); 
        rtlp2->fin = 0 ;
        rtlp2->ack = 1 ;
        rtlp2->syn = 0 ;
        rtlp2->syn_ack = 0 ;
        rtlp2->data_send = 1;
        rtlp2->check_sum = csum(rtlp2) ;
         if(sendto(s , buffer2, sizeof(buffer2) , 0, (struct sockaddr *)&dst, sizeof(src)) < 0)
          { 
            perror("sendto() error");
            exit(-1);
          }
        cout << "\n\n\n---Sending SYN_ACK pack---\n\n" << endl; 
              
         fd_set rfds;
         struct timeval tv;
         int retval;

         /* Watch stdin (fd 0) to see when it has input. */
         FD_ZERO(&rfds);
         FD_SET(s, &rfds);
         tv.tv_sec  = 1 ;
         tv.tv_usec = 0 ;
         retval = select(s+100, &rfds, NULL, NULL, &tv);
         
         if(retval == -1)
         {
           perror("select()") ;
           exit(1) ;
         } 
          
          int count = 1 ;
         while(retval <0 && count < 4)
         {
              cout << "---Sending SYN_ACK pack again---" << endl; 
              if(sendto(s , buffer2, sizeof(buffer2) , 0, (struct sockaddr *)&dst, sizeof(src)) < 0)
              { 
                perror("sendto() error in syn_ack ");
                exit(-1);
              }
              retval = select(s+100, &rfds, NULL, NULL, &tv);
         
              if(retval == -1)
             {
             perror("select()") ;
               exit(1) ;
             } 
             count++ ;
         }

         if(retval < 0)
         {
           cout << "Cannot establiish a connection(retval) " << endl ;
           exit(1) ;
         }





      }
      


      client_seq_num = rtlp->seq_num;
      client_ack_num = rtlp->ack_num;
      server_seq_num = rtlp2->seq_num;
      server_ack_num = rtlp2->ack_num;


   }




   return 0;
}