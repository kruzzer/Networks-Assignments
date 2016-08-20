#include <bits/stdc++.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netdb.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
#include <dirent.h>
#include <string.h>
#include <sys/mman.h>
#include <errno.h>
#include <signal.h>
#include <fstream>
#include <mysql.h>
#include <my_global.h>

#define MAX_SIZE 100
#define MESSAGE_SIZE 800

using namespace std ;

void client_recv(int fd , char *buffer)
{
	int r = read(fd , buffer , MAX_SIZE) ;
	if(r < 0)
	{
		perror("read me load") ;
		exit(1) ;
	}

}
//helper function
void client_send(int fd , char *buffer)
{
	int r = send(fd , buffer , MAX_SIZE , 0) ;
	if(r < 0)
	{
		perror("send me load") ;
		exit(1) ;
	}

}

int main(int argc , char * argv[] )
{
  if(argc < 3) 
  {
  	cout << "You need to give the ip address and port number of the server " << endl ;
  	exit(1) ;
  }

  int port , sockfd ,result ;
  struct sockaddr_in server_addr ;
  socklen_t addrlen ;
  port = atoi(argv[2]) ;
  sockfd =  socket(AF_INET , SOCK_STREAM , 0) ;
  if(sockfd < 0)
  {
    perror("Load in sockfd of client") ;
   	exit(1) ;
  }
  addrlen = sizeof(struct sockaddr_in) ;

  memset(&server_addr , 0 , addrlen) ;
  server_addr.sin_family = AF_INET ;
  server_addr.sin_port = htons(port) ;

  result = inet_pton(AF_INET , argv[1] , &server_addr.sin_addr) ;
  
  if(result < 0)
  {
  	perror("inet_pton me load") ;
  	exit(1) ;
  }

  result= connect (sockfd,(struct sockaddr*)&server_addr,addrlen) ; 
  if(result < 0)
  {
	printf("connect me load");
    exit(1); 
  }
  
  char buffreceive[MAX_SIZE] , buffsend[MAX_SIZE] ;

  memset(buffreceive , 0 , MAX_SIZE) ;
  client_recv(sockfd , buffreceive) ;
  cout << "Received from server : " << buffreceive << endl ;
        
  memset(buffsend , 0 , MAX_SIZE) ;
  strcpy(buffsend,"HELLO") ;
  client_send(sockfd , buffsend) ;
  
  memset(buffreceive , 0 , MAX_SIZE) ;
  client_recv(sockfd , buffreceive) ;
  cout << "Received from server : " << buffreceive << endl ;

  int choice ;

  while(1)
  {
  	cout << "Press 1 to send mails \nPress 2 to read mails \nPress any other key to exit\nEnter your choice : " ;
  	cin >> choice ;

  	memset(buffsend , 0 , MAX_SIZE) ;
    sprintf(buffsend,"%d" , choice) ;
    client_send(sockfd , buffsend) ;

    memset(buffreceive , 0 , MAX_SIZE) ;
    client_recv(sockfd , buffreceive) ;

    if(choice == 1)
    {
      while(1)
      {	
    	 cout << "Mail from ?" << endl ;
    	 memset(buffsend , 0 , MAX_SIZE) ;
    	 cin >> buffsend ;
    	 client_send(sockfd , buffsend) ;

    	 memset(buffreceive , 0 , MAX_SIZE) ;
         client_recv(sockfd , buffreceive) ;

         cout << "Mail to ?" << endl ;
    	 memset(buffsend , 0 , MAX_SIZE) ;
    	 cin >> buffsend ;
    	 client_send(sockfd , buffsend) ;

    	 memset(buffreceive , 0 , MAX_SIZE) ;
         client_recv(sockfd , buffreceive) ;

         cout<<"Input mail body [end with a single dot (.)]\n";

         while(1)
         {
        	memset(buffsend , 0 , MAX_SIZE) ;
    	    cin >> buffsend ;
    	    client_send(sockfd , buffsend) ;

         	memset(buffreceive , 0 , MAX_SIZE) ;
            client_recv(sockfd , buffreceive) ;

            if(!strcmp(buffsend , "."))
            	break ;
         }

         cout << "Mail sent successfully" << endl ;
         cout << "Do you want to send more mails ? Press Y for yes and N for no" <<endl ;
        
         memset(buffsend , 0 , MAX_SIZE) ;
    	 cin >> buffsend ;
    	 client_send(sockfd , buffsend) ;

         memset(buffreceive , 0 , MAX_SIZE) ;
         client_recv(sockfd , buffreceive) ;

         if(!strcmp(buffsend,"N"))
         {
        	cout << "Bye" << endl ;
        	break ;
         } 
      }
    }

    else if(choice == 2)
    {
    	while(1)
    	{
    		sleep(1) ;
    		while(1)
    		{ 
    		   cout << "Username ?" << endl ;
    		   memset(buffsend , 0 , MAX_SIZE) ;
    	       cin >> buffsend ;
    	       client_send(sockfd , buffsend) ;

               memset(buffreceive , 0 , MAX_SIZE) ;
               client_recv(sockfd , buffreceive) ;
               if(!strcmp(buffreceive , "OKAY"))
               	break ;
               else
               	cout << "Username does not exists . Please enter again " << endl ;         	 
           }

           while(1)
    		{ 
    		   cout << "Password ?" << endl ;
    		   memset(buffsend , 0 , MAX_SIZE) ;
    		   char *pass ;
    	       pass = getpass("Enter Password: ");
    	       strcpy(buffsend , pass) ;
    	       client_send(sockfd , buffsend) ;

               memset(buffreceive , 0 , MAX_SIZE) ;
               client_recv(sockfd , buffreceive) ;
               if(!strcmp(buffreceive , "OKAY"))
               	break ;
               else
               	cout << "Username and password do not match . Please enter again " << endl ;         	 
           }
           
           sleep(1) ;
           
           cout << "You have the following mails " << endl ;
           memset(buffreceive , 0 , MAX_SIZE) ;
           client_recv(sockfd , buffreceive) ;
           cout << "Received from server : " << buffreceive << endl ;
           int number = atoi(buffreceive) ;

           memset(buffsend , 0 , MAX_SIZE) ;
    	   strcpy(buffsend,"OKAY") ;
    	   client_send(sockfd , buffsend) ;

    	   for(int i = 0 ; i < number ; i++)
    	   {
    	   	 cout << i+1 << " from : " ;
    	   	 memset(buffreceive , 0 , MAX_SIZE) ;
             client_recv(sockfd , buffreceive) ;
             cout << buffreceive ;

             memset(buffsend , 0 , MAX_SIZE) ;
    	     strcpy(buffsend,"OKAY") ;
    	     client_send(sockfd , buffsend) ;

    	     cout << " size : " ;

    	     memset(buffreceive , 0 , MAX_SIZE) ;
             client_recv(sockfd , buffreceive) ;
             cout << buffreceive  <<" bytes"<<endl ;

             memset(buffsend , 0 , MAX_SIZE) ;
    	     strcpy(buffsend,"OKAY") ;
    	     client_send(sockfd , buffsend) ;
    	   }
           
           sleep(1) ;

    	   while(1)
    	   {
    	   	 cout << "Which one do you want to read(enter the number) ?" << endl ;
    	   	 int n ;
    	   	 cin >> n ;
    	   	 memset(buffsend , 0 , MAX_SIZE) ;
    	     sprintf(buffsend , "%d" , n) ;
    	     client_send(sockfd , buffsend) ;
             
             char msg[MESSAGE_SIZE] ;
             memset(msg , 0 , MESSAGE_SIZE) ;
             int p = read(sockfd , msg , MESSAGE_SIZE) ;

             cout << msg << endl ;

             cout << endl ;

             cout << "Do you want to read more mail ? Press Y for yes and N for no or R to refresh the list " << endl ;

             memset(buffsend , 0 , MAX_SIZE) ;
    	     cin >> buffsend ;
    	     client_send(sockfd , buffsend) ;
    	     if(!strcmp(buffsend , "N") || !strcmp(buffsend , "R"))
    	     	break ;
 
    	   }

           if(!strcmp(buffsend , "N"))
    	     	break ;          
    	}
    }

    else 
    {
    	result = close(sockfd) ;
    	if(result == -1)
    	{
    		perror("Closing socket me load") ;
    		exit(1) ;
    	}
      break ;
    }
  }
}
