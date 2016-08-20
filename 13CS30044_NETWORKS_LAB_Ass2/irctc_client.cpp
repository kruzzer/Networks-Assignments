#include <bits/stdc++.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/wait.h>
#include <fstream>
#include <ctime>
#include <stdio.h>
#include <ctype.h>

using namespace std ;

int port = 5556;
int main(int argc , char *argv[])
{
/*
	int sockfd , port_number , n ;
	struct sockaddr_in serv_addr ;
	struct hostnet *server ;

	port_number = 5555 ;
    sockfd = socket(AF_INET , SOCK_STREAM , 0) ;
    if (argc < 2) 
    {
       fprintf(stderr,"Server not mentioned .\n");
       exit(0);
    }
    
    if(sockfd < 0) 
    {
    	perror("Socket error :") ;
    	exit(0) ;
    }

     server = gethostbyname(argv[1]);

    if(server == NULL)
    {
    	fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,(char *)&serv_addr.sin_addr.s_addr, server->h_length);
     serv_addr.sin_port = htons(port_number);
     if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        perror("ERROR connecting");
*/
    //cout << "YO" << endl ;
     int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[1025];

    portno = port;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        perror("ERROR opening socket");
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }

    //cout << "YA " << endl ;

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        perror("ERROR connecting");
     n = read(sockfd,buffer,1025);
    //printf("Server:%s",buffer); 
    
    if (n < 0) 
         perror("ERROR reading from socket");
     
     cout << "Enter the path of the file : " << endl ;
     bzero(buffer,1024);
     fflush(stdin) ;
     fgets(buffer,1024,stdin);
    
     //cout << "YA " << endl ;

     int m = strlen(buffer);
     buffer[m-1] = '\0' ;

     //cout << "YA " << endl ;

     FILE *fp , *fp1;
     //cout << "YAl " << endl ;

     fp=fopen(buffer, "r");
     //cout << "YAl " << endl ;

     fp1=fopen("ticket.csv", "a");
     //cout << "YAl " << endl ;
     
     bzero(buffer,1024);
     //cout << "YAl " << endl ;

     while(fgets (buffer, 1025 , fp) != NULL) 
     {
        //bzero(buffer,1024);  
        m = strlen(buffer);
        buffer[m-1] = '\0' ;     
        //cout << buffer<<endl;
        
        n = send(sockfd,buffer,strlen(buffer),0);
        

        //cout << "yolo" << endl ;
        if (n < 0) 
         perror("ERROR writing to socket");
    
        bzero(buffer,1025);
        n = read(sockfd,buffer,1025);
        
        //printf("%s\n",buffer);
        //cout << "YA " << endl ;
        fprintf(fp1,"%s\n",buffer) ;
        cout << "Booking complete\n" ;
        //sleep(5);
        bzero(buffer,1024);


      } 
        fclose(fp) ;
        fclose(fp1) ;
        close(sockfd);
    
    return 0 ;

}