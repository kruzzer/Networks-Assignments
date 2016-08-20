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

//helper function
void server_recv(int fd , char *buffer)
{
	int r = read(fd , buffer , MAX_SIZE) ;
	if(r < 0)
	{
		perror("recv me load") ;
		exit(1) ;
	}

}
//helper function
void server_send(int fd , char *buffer)
{
	int r = send(fd , buffer , MAX_SIZE , 0) ;
	if(r < 0)
	{
		perror("send me load") ;
		exit(1) ;
	}

}

struct email_info
{
	char from[MAX_SIZE] ;
	char message[MESSAGE_SIZE] ;
	int size ;
} ;

struct email_info emails_send[200] ;



// Mysql handling functions 

//--------------------------------------------------------
//--------------------------------------------------------

int check_name(char username[] )
{
  MYSQL *con = mysql_init(NULL) ;
  int columns ;

  if(con == 0)
  {
    cout << "mysql error" << endl ;
    exit(1) ;
  }

  if(mysql_real_connect(con , "10.5.18.68" , "13CS30044" , "cse12" , "13CS30044" , 0 , NULL , 0) == NULL)
  {
    cout << "Connection error " << endl ;
    exit(1) ;
  }

  if( mysql_query(con, "SELECT * FROM abc") ) 
  {
    cout << "mysql_query error" << endl ;
    exit(1) ;
  }

  MYSQL_RES * result =  mysql_store_result(con) ;

  if(result == NULL) 
  {
    cout << "result error" << endl ;
    exit(1) ; 
  }

    columns = mysql_num_fields(result) ;

    MYSQL_ROW row;

    while ((row = mysql_fetch_row(result)))
    {
      if(!strcmp(row[0],username) )
      {
        mysql_free_result(result) ;
        mysql_close(con) ;
        return 1 ;
      }

    }

    mysql_free_result(result) ;
    mysql_close(con) ;
    return 0 ;
}



int check_user(char username[] , char password[])
{
	MYSQL *con = mysql_init(NULL) ;
	int columns ;

	if(con == 0)
	{
		cout << "mysql error" << endl ;
		exit(1) ;
	}

	if(mysql_real_connect(con , "10.5.18.68" , "13CS30044" , "cse12" , "13CS30044" , 0 , NULL , 0) == NULL)
	{
		cout << "Connection error " << endl ;
		exit(1) ;
	}

	if( mysql_query(con, "SELECT * FROM abc") ) 
	{
	  cout << "mysql_query error" << endl ;
	  exit(1) ;
	}

	MYSQL_RES * result =  mysql_store_result(con) ;

	if(result == NULL) 
	{
	  cout << "result error" << endl ;
	  exit(1) ;	
	}

    columns = mysql_num_fields(result) ;

    MYSQL_ROW row;

    while ((row = mysql_fetch_row(result)))
    {
    	if(!strcmp(row[0],username) && !strcmp(row[1],password))
    	{
    		mysql_free_result(result) ;
    		mysql_close(con) ;
    		return 1 ;
    	}

    }

    mysql_free_result(result) ;
    mysql_close(con) ;
    return 0 ;
}



int get_emails(char username[])
{
	int n = 0 ;

	MYSQL *con = mysql_init(NULL) ;
	int columns ;

	if(con == 0)
	{
		cout << "mysql error" << endl ;
		exit(1) ;
	}

	if(mysql_real_connect(con , "10.5.18.68" , "13CS30044" , "cse12" , "13CS30044" , 0 , NULL , 0) == NULL)
	{
		cout << "Connection error " << endl ;
		exit(1) ;
	}

	if( mysql_query(con, "SELECT * FROM abc_mail ORDER BY id DESC") ) 
	{
	  cout << "mysql_query error" << endl ;
	  exit(1) ;
	}

	MYSQL_RES * result =  mysql_store_result(con) ;

	if(result == NULL) 
	{
	  cout << "result error" << endl ;
	  exit(1) ;	
	}

	columns = mysql_num_fields(result) ;

    MYSQL_ROW row ;

    while((row = mysql_fetch_row(result)))
    {
    	if(!strcmp(row[2] , username))
    	{
    		strcpy(emails_send[n].from , row[1]) ;
    		strcpy(emails_send[n].message , row[3]) ;
    		emails_send[n].size = strlen(emails_send[n].message) ;
    		n++ ;
    	}

    }

    cout << "value of n : " << n << endl ;

	return  n ;
}

void my_sql_same( char from_email[] , char to_email[] , char message[] ) 
{
    //cout << "In sql" << endl ;
     MYSQL *con = mysql_init(NULL) ;
	int columns ;

	if(con == 0)
	{
		cout << "mysql error" << endl ;
		exit(1) ;
	}

	if(mysql_real_connect(con , "10.5.18.68" , "13CS30044" , "cse12" , "13CS30044" , 0 , NULL , 0) == NULL)
	{
		cout << "Connection error " << endl ;
		exit(1) ;
	}

	char QUERRY[4000] ;
    strcpy(QUERRY , "INSERT INTO abc_mail(from_email , to_email , message) VALUES('") ;
  strcat(QUERRY , from_email) ;
  strcat(QUERRY , "','") ;
  strcat(QUERRY , to_email) ;
  strcat(QUERRY , "','") ;
  strcat(QUERRY , message) ;
  strcat(QUERRY , "')") ;
   
  //cout << QUERRY << endl ;
  if( mysql_query(con, QUERRY) ) 
  {
    cout << "mysql_query error" << endl ;
    exit(1) ;
  }

  mysql_close(con) ;
}

void my_sql_other( char from_email[] , char to_email[] , char message[] , int port_num , char IP[])
{
	int sockfd , result ;
	struct sockaddr_in server_addr ;
    socklen_t addrlen ;
    
    addrlen = sizeof(struct sockaddr_in) ;

	sockfd = socket(AF_INET , SOCK_STREAM , 0) ;
	memset(&server_addr , 0 , addrlen) ;

	server_addr.sin_family = AF_INET ;
	server_addr.sin_port = htons(port_num) ;

	result = inet_pton(AF_INET,IP,&server_addr.sin_addr) ;
	if (result <= 0)
    {
        perror ("server behaving as client me load :inet_pton") ;
        exit (1) ;
    }

    result= connect(sockfd , (struct sockaddr *)&server_addr , addrlen );
	if(result < 0)
	{
		printf("server behaving as client me load :connect") ;
		exit(1) ; 
	}
    
    char buffreceive[MAX_SIZE] , buffsend[MAX_SIZE] ;

    memset(buffreceive , 0 , MAX_SIZE) ;
    server_recv(sockfd , buffreceive) ;
    cout << "Received from server : " << buffreceive << endl ;
        
    memset(buffsend , 0 , MAX_SIZE) ;
    strcpy(buffsend,"HELLO") ;
    server_send(sockfd , buffsend) ;
  
    memset(buffreceive , 0 , MAX_SIZE) ;
    server_recv(sockfd , buffreceive) ;
    cout << "Received from server : " << buffreceive << endl ;

    memset(buffsend , 0 , MAX_SIZE) ;
    strcpy(buffsend,"1") ;
    server_send(sockfd , buffsend) ;

    memset(buffreceive , 0 , MAX_SIZE) ;
    server_recv(sockfd , buffreceive) ;

    memset(buffsend , 0 , MAX_SIZE) ;
    strcpy(buffsend,from_email) ;
    server_send(sockfd , buffsend) ;

    memset(buffreceive , 0 , MAX_SIZE) ;
    server_recv(sockfd , buffreceive) ;
    
    memset(buffsend , 0 , MAX_SIZE) ;
    strcpy(buffsend , to_email) ;
    server_send(sockfd , buffsend) ;
    
    memset(buffreceive , 0 , MAX_SIZE) ;
    server_recv(sockfd , buffreceive) ;

    int len = strlen(message) ;
    int i , k = 0 ;

    while(1)
    {
    	memset(buffsend , 0 , MAX_SIZE) ;

    	for( i = 0 ; i < 99 && k < len ; i++)
    	{
    		buffsend[i] = message[k] ;
    		k++ ;
    	}

    	buffsend[i] = '\0' ;

    	server_send(sockfd , buffsend) ;
    
        memset(buffreceive , 0 , MAX_SIZE) ;
        server_recv(sockfd , buffreceive) ;

        if(k >= len) break ;
    }

    memset(buffsend , 0 , MAX_SIZE) ;
    strcpy(buffsend , ".") ;
    server_send(sockfd , buffsend) ;

    memset(buffreceive , 0 , MAX_SIZE) ;
    server_recv(sockfd , buffreceive) ;

    memset(buffsend , 0 , MAX_SIZE) ;
    strcpy(buffsend , "N") ;
    server_send(sockfd , buffsend) ;

    memset(buffsend , 0 , MAX_SIZE) ;
    strcpy(buffsend , "3") ;
    server_send(sockfd , buffsend) ;

    memset(buffreceive , 0 , MAX_SIZE) ;
    server_recv(sockfd , buffreceive) ;

  
}









int main(int argc , char * argv[])
{
	if(argc < 4)
	{
		cout << "you need to give arguments <IP_ADDRESS of xyz> <port for client> <port for xyz>" << endl ;
		exit(1) ;
	}

	char IP[100] ;
	int port1 , port2 ,result , choice ;
	int newsockfd ;
	pid_t pid ;
	char buffsend[MAX_SIZE] , buffreceive[MAX_SIZE] , from_email[MAX_SIZE] , to_email[MAX_SIZE] , message[MESSAGE_SIZE];
	char username[MAX_SIZE]  , password[MAX_SIZE] ;
	strcpy(IP , argv[1]) ;
	port1 = atoi(argv[2]) ;
	port2 = atoi(argv[3]) ;

    struct sockaddr_in server_addr , client_addr ; socklen_t addrlen , clilen;
    

    int sockfd = socket(AF_INET , SOCK_STREAM , 0 ) ;

    if(sockfd < 0)
    {
    	perror("Load in sockfd of client") ;
   	    exit(1) ;
    }


    addrlen = sizeof(struct sockaddr_in) ;
    memset(&server_addr , 0 , addrlen) ;
    memset(&client_addr , 0 , addrlen) ; 

    server_addr.sin_family = AF_INET ;
    server_addr.sin_addr.s_addr = INADDR_ANY ;
    server_addr.sin_port = htons(port1) ;

    if(bind(sockfd , (struct sockaddr *)&server_addr , sizeof(server_addr)) < 0 )
    {
    	perror("Error in binding the TCP socket") ;
   	    exit(1) ;
    }

    result = listen(sockfd , 20) ;
    clilen = sizeof(client_addr) ;

    while(1)
    {
       newsockfd = accept(sockfd , (struct sockaddr *)&client_addr , &clilen) ;
       cout << "Connection from an abc client accepted " << endl ;
       
       pid = fork() ;

       if(pid == 0)
       {
       	// Handling client 
        memset(buffsend , 0 , MAX_SIZE) ;
        strcpy(buffsend,"Service Ready") ;
        server_send(newsockfd , buffsend) ;

       	memset(buffreceive , 0 , MAX_SIZE) ;
        server_recv(newsockfd , buffreceive) ;
        cout << "Received from client : " << buffreceive << endl ;
        
        memset(buffsend , 0 , MAX_SIZE) ;
        strcpy(buffsend,"OK") ;
        server_send(newsockfd , buffsend) ;
        
        while(1)
        {
        	memset(buffreceive , 0 , MAX_SIZE) ;
            server_recv(newsockfd , buffreceive) ;
            cout << "Received from client : " << buffreceive << endl ;
            choice = atoi(buffreceive) ;
            memset(buffsend , 0 , MAX_SIZE) ;
         	strcpy(buffsend,"OKAY") ;
        	server_send(newsockfd , buffsend) ;

            if(choice == 1)
            {
            	while(1)
            	{
            		memset(buffreceive , 0 , MAX_SIZE) ;
                    server_recv(newsockfd , buffreceive) ;
                    cout << "Received from client : " << buffreceive << endl ;
                    strcpy(from_email , buffreceive) ;

                    memset(buffsend , 0 , MAX_SIZE) ;
         			strcpy(buffsend,"OKAY") ;
        			server_send(newsockfd , buffsend) ;

        			memset(buffreceive , 0 , MAX_SIZE) ;
                    server_recv(newsockfd , buffreceive) ;
                    cout << "Received from client : " << buffreceive << endl ;
                    strcpy(to_email , buffreceive) ;

                    memset(buffsend , 0 , MAX_SIZE) ;
         			strcpy(buffsend,"OKAY") ;
        			server_send(newsockfd , buffsend) ;
                    
                    strcpy(message , "") ;

                    while(1)
                    {
                    	  memset(buffreceive , 0 , MAX_SIZE) ;
                        server_recv(newsockfd , buffreceive) ;
                        cout << "Received from client : " << buffreceive << endl ;
                        

                        memset(buffsend , 0 , MAX_SIZE) ;
         			          strcpy(buffsend,"OKAY") ;
        			          server_send(newsockfd , buffsend) ;
                        if(!strcmp(buffreceive , "."))
        			    	     break ;
                        strcat(message , buffreceive) ;
                        strcat(message , " ") ;

                    }
                   // cout <<"ya" << endl;
                   // cout << to_email + strlen(to_email)-8 << endl ;
 
                    if( !strcmp(&to_email[strlen(to_email)-8] , "@abc.com") )
                    {	
                      cout << "here in the same domain " << endl ;
                      cout << message << endl ;
                    	my_sql_same(from_email , to_email , message) ;
                    }
                    else
                    {
                    	my_sql_other(from_email , to_email , message , port2 , IP) ;
                    }

                    memset(buffreceive , 0 , MAX_SIZE) ;
                    server_recv(newsockfd , buffreceive) ;
                    cout << "Received from client : " << buffreceive << endl ;
                        

                    memset(buffsend , 0 , MAX_SIZE) ;
         			     strcpy(buffsend,"OKAY") ;
        			     server_send(newsockfd , buffsend) ;
                    

                    if(!strcmp(buffreceive , "N"))
                    {
                    	break ;
                    }
            	}
            }

            else if(choice == 2) 
            { 
              while(1)
              {	
            	while(1)
            	{
            		memset(buffreceive , 0 , MAX_SIZE) ;
                    server_recv(newsockfd , buffreceive) ;
                    cout << "Username from client : " << buffreceive << endl ;
                    int check = check_name(buffreceive) ;
                    

                    memset(buffsend , 0 , MAX_SIZE) ;
         			if(check == 1)
         				{ 
         					strcpy(buffsend,"OKAY") ;
         					strcpy(username , buffreceive) ;
         				}
         			else	strcpy(buffsend,"NOT") ;
        			server_send(newsockfd , buffsend) ;

        			if(check == 1) break ;
            	}

            	while(1)
            	{
            		memset(buffreceive , 0 , MAX_SIZE) ;
                    server_recv(newsockfd , buffreceive) ;
                    cout << "Password from client : " << buffreceive << endl ;
                    int check = check_user(username , buffreceive) ;
                    

                    memset(buffsend , 0 , MAX_SIZE) ;
         			if(check == 1)
         				{
         					strcpy(buffsend,"OKAY") ;
         					strcpy(password , buffreceive) ;
         				}
         			else	strcpy(buffsend,"NOT") ;
        			server_send(newsockfd , buffsend) ;

        			if(check == 1) break ;
            	}

            	sleep(1) ;

                int number = get_emails(username) ;

                memset(buffsend , 0 , MAX_SIZE) ;
                sprintf(buffsend,"%d",number) ;
                server_send(newsockfd , buffsend) ;

                memset(buffreceive , 0 , MAX_SIZE) ;
                server_recv(newsockfd , buffreceive) ;

                for(int i = 0 ; i < number ; i++)
                {
             	  memset(buffsend , 0 , MAX_SIZE) ;
                  strcpy(buffsend,emails_send[i].from) ;
                  server_send(newsockfd , buffsend) ;

                  memset(buffreceive , 0 , MAX_SIZE) ;
                  server_recv(newsockfd , buffreceive) ;

                  memset(buffsend , 0 , MAX_SIZE) ;
                  sprintf(buffsend , "%d" , emails_send[i].size ) ;
                  server_send(newsockfd , buffsend) ;

                  memset(buffreceive , 0 , MAX_SIZE) ;
                  server_recv(newsockfd , buffreceive) ;
                }

                sleep(1);

                while(1)
                {
             	  memset(buffreceive , 0 , MAX_SIZE) ;
                  server_recv(newsockfd , buffreceive) ;
                 
                  int n = atoi(buffreceive) ;

                  if(n > number)
                  {
                 	char buff[MESSAGE_SIZE] ;
                 	strcpy(buff,"Please enter correct number") ;
                 	int p =  send(newsockfd , buff , MESSAGE_SIZE , 0) ;
                  }
                  else
                  {
                 	int p = send(newsockfd , emails_send[n-1].message , MESSAGE_SIZE , 0) ;
                  }

                  memset(buffreceive , 0 , MAX_SIZE) ;
                  server_recv(newsockfd , buffreceive) ;
                  cout << "Received from client : " << buffreceive << endl ;
              
                  if(!strcmp(buffreceive,"N") || !strcmp(buffreceive,"R") )
                  	break ;
                }

                if(!strcmp(buffreceive,"N"))
                  	break ;
              }
            }

            else
            	//exit(1) ;
              break ;      

        }

        exit(1) ;
       }
    }
}

