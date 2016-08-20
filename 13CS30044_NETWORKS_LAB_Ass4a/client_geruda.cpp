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
#include <assert.h>
#include <fstream>
#include <string>

#define MAX_SIZE 1024
// g++ server_abc_2.cpp  -o mysql -I/usr/include/mysql -lmysqlclient

using namespace std ;

void client_recv(int fd , char *buffer)
{
	//cout <<"recv here" << endl ;
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
	//cout << "send here" << endl ;
	int r = send(fd , buffer , strlen(buffer) , 0) ;
	if(r < 0)
	{
		perror("send me load") ;
		exit(1) ;
	}

}

char IP[100] ; 
int port_smtp , port_pop ;

void smtp_protocol() ;
void pop_protocol() ;

int main()
{
	strcpy(IP,"10.5.30.131") ;
	port_smtp = 25 ;
	port_pop = 110 ;

	while(1)
	{
		int choice ;
		sleep(2) ;	
		cout << "\n\nPress 1 to send mails \nPress 2 to read mails\nAny other key to exit\nEnter your choice : " ;
		cin >> choice ;
		if(choice == 1)
			smtp_protocol() ;
		else if(choice == 2)
			pop_protocol() ;
		else
			{
				cout << "\nBye\n" ;
				exit(1) ;
			}	       

	}

}


void smtp_protocol()
{
	struct sockaddr_in server_addr ;
	socklen_t addrlen ;
	int sockfd , result ;

	sockfd = socket(AF_INET , SOCK_STREAM , 0) ;
	
	if (sockfd < 0)
	{
		perror("smtp_protocol sockfd load") ;
		exit(1) ;
	}

	addrlen = sizeof(struct sockaddr_in) ;

	memset(&server_addr , 0 , addrlen) ;
	server_addr.sin_family = AF_INET ;
	server_addr.sin_port = htons(port_smtp) ;
	result = inet_pton(AF_INET , IP , &server_addr.sin_addr) ;

	if(result < 0)
	{
		perror("smtp_protocol inet_pton load") ;
		exit(1) ;
	}

	result = connect(sockfd , (struct sockaddr*)&server_addr , addrlen) ;

	if(result < 0)
	{
		perror("smtp_protocol connect load") ;
		exit(1) ;
	}

	char buffreceive[MAX_SIZE] , buffsend[MAX_SIZE] ;
	char mail_from[MAX_SIZE] , mail_to[MAX_SIZE] ;

	memset(buffreceive , 0 , MAX_SIZE) ;
	client_recv(sockfd , buffreceive) ;
	cout << "Received from server : " << buffreceive ;

	memset(buffsend , 0 , MAX_SIZE) ;
	strcpy(buffsend , "HELO localhost.localdomain\r\n") ;
	client_send(sockfd , buffsend) ;

	memset(buffreceive , 0 , MAX_SIZE) ;
	client_recv(sockfd , buffreceive) ;
	cout << "Received from server : " << buffreceive  ;
    while(1)
    {	
		cout << "Mail from : " ;
		cin >> mail_from ;
		memset(buffsend , 0 , MAX_SIZE) ;
		sprintf(buffsend , "MAIL FROM:<%s>\r\n" , mail_from) ;
		//cout << buffsend  ;
		client_send(sockfd , buffsend) ;

		memset(buffreceive , 0 , MAX_SIZE) ;
		client_recv(sockfd , buffreceive) ;
		cout << "Received from server : " << buffreceive ;

		cout << "Mail to : " ;
		cin >> mail_to ;
		memset(buffsend , 0 , MAX_SIZE) ;
		sprintf(buffsend , "RCPT TO:<%s>\r\n" , mail_to) ;
		//cout << buffsend  ;
		client_send(sockfd , buffsend) ;

		memset(buffreceive , 0 , MAX_SIZE) ;
		client_recv(sockfd , buffreceive) ;
		cout << "Received from server : " << buffreceive ;

		memset(buffsend , 0 , MAX_SIZE) ;
		strcpy(buffsend , "DATA\r\n") ;
		//cout << buffsend  ;
		client_send(sockfd , buffsend) ;

		memset(buffreceive , 0 , MAX_SIZE) ;
		client_recv(sockfd , buffreceive) ;
		cout << "Received from server : " << buffreceive ;
    
    	char line[MAX_SIZE] ;
    
		while(1)
		{
			memset(line , 0 , MAX_SIZE) ;
			cin.getline(line , sizeof(line)) ;
	    	memset(buffsend , 0 , MAX_SIZE) ;
	    	strcpy(buffsend , line) ;
	    	strcat(buffsend,"\r\n") ;
	    	//cout << buffsend  ;
	    	client_send(sockfd , buffsend) ;
	    	if(line[0] == '.')
	    		break ;
		}

		//cout << "i am here " << endl ;

		memset(buffreceive , 0 , MAX_SIZE) ;
		client_recv(sockfd , buffreceive) ;
		cout << "Received from server : " << buffreceive ;

		cout << "Do you want to send more mails ? Press Y for YES or N for NO" << endl ;
		memset(line , 0 , MAX_SIZE) ;
		cin >> line ;
		assert(line[0] == 'Y' || line[0] == 'N') ;
		if(line[0] == 'N')
			break ; 

	}

		memset(buffsend , 0 , MAX_SIZE) ;
		strcpy(buffsend , "QUIT\r\n") ;
		//cout << buffsend  ;
		client_send(sockfd , buffsend) ;

		memset(buffreceive , 0 , MAX_SIZE) ;
		client_recv(sockfd , buffreceive) ;
		cout << "Received from server : " << buffreceive ;

		close(sockfd) ; 
}


void pop_protocol()
{
	struct sockaddr_in server_addr ;
	socklen_t addrlen ;
	int sockfd , result ;

	sockfd = socket(AF_INET , SOCK_STREAM , 0) ;
	
	if (sockfd < 0)
	{
		perror("pop_protocol sockfd load") ;
		exit(1) ;
	}

	addrlen = sizeof(struct sockaddr_in) ;

	memset(&server_addr , 0 , addrlen) ;
	server_addr.sin_family = AF_INET ;
	server_addr.sin_port = htons(port_pop) ;
	result = inet_pton(AF_INET , IP , &server_addr.sin_addr) ;

	if(result < 0)
	{
		perror("pop_protocol inet_pton load") ;
		exit(1) ;
	}

	result = connect(sockfd , (struct sockaddr*)&server_addr , addrlen) ;

	if(result < 0)
	{
		perror("pop_protocol connect load") ;
		exit(1) ;
	}

	char buffreceive[MAX_SIZE] , buffsend[MAX_SIZE] ;
	char user[MAX_SIZE] , password[MAX_SIZE] ;

	memset(buffreceive , 0 , MAX_SIZE) ;
	client_recv(sockfd , buffreceive) ;
	cout << "Received from server : " << buffreceive ;
	
	memset(user , 0 , MAX_SIZE) ;
	cout << "Enter user name : " ;
	cin >> user ;

	memset(buffsend , 0 , MAX_SIZE) ;
	sprintf(buffsend , "USER %s\r\n" , user) ;
	//cout << buffsend  ;
	client_send(sockfd , buffsend) ;

	memset(buffreceive , 0 , MAX_SIZE) ;
	client_recv(sockfd , buffreceive) ;
	cout << "Received from server : " << buffreceive ;

	memset(buffsend , 0 , MAX_SIZE) ;
	char *pass ;
	pass = getpass("Enter Password: ");
	sprintf(buffsend , "PASS %s\r\n" , pass) ;
	//cout << buffsend  ;
	client_send(sockfd , buffsend) ;

	memset(buffreceive , 0 , MAX_SIZE) ;
	client_recv(sockfd , buffreceive) ;
	cout << "Received from server : " << buffreceive ;

    while(1)
    {
		memset(buffsend , 0 , MAX_SIZE) ;
		strcpy(buffsend , "LIST\r\n") ;
		//cout << buffsend  ;
		client_send(sockfd , buffsend) ;

		memset(buffreceive , 0 , MAX_SIZE) ;
		client_recv(sockfd , buffreceive) ;
		cout << "\nReceived from server : " << buffreceive ;



		memset(buffreceive , 0 , MAX_SIZE) ;
		client_recv(sockfd , buffreceive) ;
		cout << buffreceive ;

		memset(buffreceive , 0 , MAX_SIZE) ;
		client_recv(sockfd , buffreceive) ;
		cout << buffreceive ;



		cout<<"Hi"<<endl ;

		int id_mail ;
		cout << "Which mail you want to read ? Enter the id : " ;		
		cin >> id_mail ;  
		memset(buffsend , 0 , MAX_SIZE) ;
		sprintf(buffsend , "RETR %d\r\n" , id_mail) ;
		cout << buffsend  ;
		client_send(sockfd , buffsend) ;
        char  buf[MAX_SIZE] ;
        memset(buf , 0 , MAX_SIZE) ;
        int y = 0 ;
        while(1)
        {
        	cout << "yo" << endl ;

	      int r = read(sockfd , &buf[y] , 1) ;
	      if(r < 0)
	      {
		      perror("read me load") ;
		       exit(1) ;
	       }
	       if(buf[y] == '\r') break ;
	       if(buf[y] != '\0') y++ ;
	       
	       //cout << buf[y-1] ;
	       //cout << "r = " << r << endl ;
        }
             buf[y+1] = '\0' ;	
       		 cout << buf << endl ;
		cout << "\n\nDo you want to read more mails ? Press Y for yes and N for No " ;
		
		char line[5] ;
		memset(line , 0 , MAX_SIZE) ;
		cin >> line ;
		assert(line[0] == 'Y' || line[0] == 'N') ;
		if(line[0] == 'N')
			break ; 
	}
}