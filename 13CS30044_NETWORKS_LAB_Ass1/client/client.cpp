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



#define MAX_SIZE 1024
#define MAX_BODY 1000
#define MAX_HEADLINE 500
#define BUF_SIZE 30

using namespace std;

/*
https://drive.google.com/folderview?id=0B5FPngw5LhnCd1B3S1lYWDQxaU0&usp=sharing

http://www.linuxhowtos.org/C_C++/socket.htm
*/


struct record 
{

  char headline[MAX_HEADLINE];
  time_t dateTime;
  char body[MAX_BODY];
  int type; // ACADEMIC / NON_ACADEMIC
};


class Reporter
{

private:
  int sockfd,portno;
  char* serverIP;
  struct sockaddr_in serv_addr;

  void error(const char* error_mssg)
    {
      perror(error_mssg);
      exit(1);
    }

public:
  Reporter(char* serverIP,int port_num )
  {
    this->serverIP=strdup(serverIP);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if(sockfd<0)
      error("Error in opening Client Socket");

    this->portno=port_num;

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port_num);
    inet_pton(AF_INET, serverIP, &(serv_addr.sin_addr));
  }


  void WriteArticle()
  {

    system("clear");
    int checkconnect = connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if (checkconnect < 0)
    {
      error("Error in connection");
    }
    else
    {
      cout<<"...  Connected to the main server .\n";
    }

    struct record newsObject;
    int chosenOption =0;

    while(chosenOption!=1 && chosenOption!=2)
    {
      cout<<"\n*******************************************************************************************************" ;
      cout<<"\n******=>> Press 1 for Academic related reports      ..!************************************************" ;
      cout<<"\n******=>> Press 2 for Non-Academic related reports  ..!************************************************" ;
      cout<<"\n*******************************************************************************************************\n\n" ;
      
      cout << "Enter the index corresponding the access you desire : " ;
      cin >> chosenOption;
    }
    if(chosenOption == 1)
    {
      newsObject.type = 0;  
    }
    else
    {
      newsObject.type = 1; 
    }
    /*
      http://stackoverflow.com/questions/22444999/c-reading-a-a-txt-file-line-by-line-into-a-char-array-within-an-array-of-struc
    */
    char b ;
    cin.get(b);
    cout<<"------------------------ Enter the Headline ! ----------------------------------------------------------------\n";
    cin.getline(newsObject.headline,MAX_HEADLINE);
    cout<<"--------------------------------------------------------------------------------------------------------------\n";
    cout<<"------------------------ Enter the Body ! --------------------------------------------------------------------\n";
    cout<<"Enter the body !\n";
    cout<<"--------------------------------------------------------------------------------------------------------------\n";
    cin.getline(newsObject.body,MAX_BODY);
    int ji = 0;
    int writeChecker; 
    if(chosenOption == 1)
    {
      char *sendNews;
      sendNews = (char*)malloc(sizeof(char)*500);
      memset (sendNews, 0,500);
      strcpy(sendNews,"/academic");
      writeChecker = write(sockfd,sendNews,500);
      if(writeChecker<0)
      {
        error("Error in sending news!");
      } 
    }
    else
    {
      char *sendNews;
      sendNews = (char*)malloc(sizeof(char)*500);
      memset (sendNews, 0,500);
      strcpy(sendNews,"/nacademic");
      printf("%s\n",sendNews);
      writeChecker = write(sockfd,sendNews,500);
      if(writeChecker<0)
      {
        error("Error in sending news!");
      }  
    }
    char *sendNews;
    sendNews = (char*)malloc(sizeof(char)*500);
    memset (sendNews, 0,500);
    strcpy(sendNews,newsObject.headline);
    writeChecker = write(sockfd,sendNews,500);
    if(writeChecker<0)
    {
      error("Error in sending news!");
    } 
    memset (sendNews, 0,500);
    strcpy(sendNews,newsObject.body);
    //printf("%s\n",sendNews);
    writeChecker = write(sockfd,sendNews,500);
    if(writeChecker<0)
    {
      error("Error in sending news!");
    }    
    else{
        //system("clear");
        cout<<"\n---------------A News report on has been succesfully sent to TCP Server---------------------------------\n\n";
      }

      close(sockfd);
  }
};

class Reader
{
private:
  int sockfd,portno;
  char* serverIP;
  struct sockaddr_in serv_addr;

  void error(const char* error_mssg)
    {
      perror(error_mssg);
      exit(1);
    }

  char* showinXterm(char *headline,char *dateOfHeadline,char *bodyOfNews)  // returns the name of the temporary file_name
  {
    ofstream out;
    out.open("temp.txt",ios::out | ios::trunc);
    out<<"-------------------------------------------------------------------------------\n" ;
    out<<"-------------------------------------------------------------------------------\n" ;
    out<<"HEADLINE : " << headline << endl;
    out<<"DATE     : " << dateOfHeadline << endl;
    out<<"-------------------------------------------------------------------------------\n" ;
    out<<"-------------------------------------------------------------------------------\n" ;
    out<<"                               NEWS BODY                                       \n" 
    << bodyOfNews << "\n";
    out<<"-------------------------------------------------------------------------------\n" ;
    out<<"-------------------------------------------------------------------------------\n" ;

    out.close();
    return strdup("temp.txt");
  }

public:
  Reader(char* serverIP,int port_num )
  {
    this->serverIP=strdup(serverIP);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if(sockfd<0)
      error("Error in opening Client Socket");

    this->portno=port_num;

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port_num);

    inet_pton(AF_INET, serverIP, &(serv_addr.sin_addr));
  
  }

  void ReadArticle()
  {
    
    system("clear");
    int checkconnect = connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if (checkconnect < 0)
    {
      error("Error in connection");
    }
    else
    {
      cout<<"...  Connected to the main server .\n";
    }
    int newstypeKeeper = 0;
    char headLinelist[25][500];
    char dateLinelist[25][500];
    char bodyNewslist[25][500];
    int numberOfNews=0;

    struct record newsObject;
    int chosenOption=0;

    while(chosenOption!=1 && chosenOption!=2)
    {
      cout<<"\n*******************************************************************************************************" ;
      cout<<"\n******=>> Press 1 for Academic related reports      ..!************************************************" ;
      cout<<"\n******=>> Press 2 for Non-Academic related reports  ..!************************************************" ;
      cout<<"\n*******************************************************************************************************\n\n" ;
      cout << "Enter the index corresponding to your particular choice : " << endl ;
      cin >> chosenOption;  
    }
    newstypeKeeper = chosenOption;
    int writeChecker;
    if(chosenOption == 1)
    {
      char *sendNews;
      sendNews = (char*)malloc(sizeof(char)*500);
      memset (sendNews, 0,500);
      strcpy(sendNews,"/academic");
      writeChecker = write(sockfd,sendNews,500);
      if(writeChecker<0)
      {
        error("Error in sending news!");
      } 
    }
    else
    {
      char *sendNews;
      sendNews = (char*)malloc(sizeof(char)*500);
      memset (sendNews, 0,500);
      strcpy(sendNews,"/nacademic");
      //printf("%s\n",sendNews);
      writeChecker = write(sockfd,sendNews,500);
      if(writeChecker<0)
      {
        error("Error in sending news!");
      }  
    }
    char *sendNews;
    sendNews = (char*)malloc(sizeof(char)*500);
      memset (sendNews, 0,500);
    
      int readNewsFiles=read(sockfd,sendNews,500);

      while(readNewsFiles>0)
      {
        strcpy(headLinelist[numberOfNews],sendNews);
        memset (sendNews, 0,500);
        readNewsFiles=read(sockfd,sendNews,500);
        strcpy(dateLinelist[numberOfNews],sendNews);
        memset (sendNews, 0,500);
        readNewsFiles=read(sockfd,sendNews,500);
        strcpy(bodyNewslist[numberOfNews],sendNews);
        memset (sendNews, 0,500);
        readNewsFiles=read(sockfd,sendNews,500);
        
        numberOfNews = numberOfNews + 1;
      }
/*
      readNewsFiles=read(sockfd,sendNews,500);
      numberOfNews = 0;
      while(readNewsFiles>0)
      {
        strcpy(dateLinelist[numberOfNews],sendNews);
        memset (sendNews, 0,500);
        readNewsFiles=read(sockfd,sendNews,500);
        numberOfNews = numberOfNews + 1;
      }
*/


      if(numberOfNews==0)
      {
        cout<<"\n\nNo news entries published yet !\n"<<endl;
        close(sockfd);
        return;
      }

      for(int i=numberOfNews-1;i>=0;i--)
      {
        cout<<"\nArticle No : " << (i+1 )<<"\nDate : "<< dateLinelist[i] <<"Headline : "<< headLinelist[i] <<endl <<endl;
      }
      chosenOption = 0;
      while(chosenOption<1 || chosenOption>numberOfNews)
      {
        cout<<"\n\nEnter the headline index you want to display the news for: ";
        cin >> chosenOption;
      }
      
      int pid=fork();

      if(pid==0)
      {
        
      close(sockfd);

      char* filename = showinXterm(headLinelist[chosenOption-1],dateLinelist[chosenOption-1],bodyNewslist[chosenOption-1]);

      //display the contents of the (opt-1)th news
      int rtv=execlp("xterm","xterm","-hold","-e","more",filename,(const char*)NULL);

      if(rtv==-1)
        error("Error in running xterm !\n");
    }
      wait(NULL);

      close(sockfd);
  }
};

class Administrator
{

private:
  int sockfd,portno;
  char* serverIP;
  struct sockaddr_in serv_addr;

  void error(const char* error_mssg)
    {
      perror(error_mssg);
      exit(1);
    }

public:
  Administrator(char* serverIP,int port_num )
  {
      this->serverIP=strdup(serverIP);
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if(sockfd<0)
      error("Error in opening Client Socket");

    this->portno=port_num;

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port_num);

    inet_pton(AF_INET, serverIP, &(serv_addr.sin_addr));

  }


  
  void newsAdmin()
  {
    int rst;
    char *date;
    int flags = 0;
    date=(char*)malloc(sizeof(char*)*100);
    printf("Enter days before which news you want to delete\n");
    scanf("%s",date);
    /* Sending message to the server. */
    rst = sendto (sockfd, date, sizeof(date), flags, (struct sockaddr *) &serv_addr, 
                    sizeof (struct sockaddr_in)); /* Value of rst is 20,
        * on successful transmission; i.e. It has nothing to do with a 
        * NULL terminated string.
        */
    if (rst < 0)
    {
        perror ("Client Sendto 1 failed");
        exit (1);
    }
    //printf("%d==size sent\n",rst);
   
    while(1)
    {
        char buf[BUF_SIZE] = {'\0'};
        struct sockaddr_in sender_addr;
        socklen_t sender_len;
        /* Receive a message from the server. */
        rst = recvfrom (sockfd, buf, BUF_SIZE, flags, 
                        (struct sockaddr *) &sender_addr, 
                        &sender_len);
        if (rst < 0)
        {
            perror ("Client did not receive");
            exit (1);
        }
        if(strcmp(buf,"Enter Password\n")!=0)
        {
          printf("%s",buf);
            break;
        }
        else
        {

            printf("%s",buf);
            char *pass;
            pass=(char*)malloc(sizeof(char)*100);
            scanf("%s",pass);
            rst = sendto (sockfd, pass, 100, flags, (struct sockaddr *) &serv_addr, 
                        sizeof (struct sockaddr_in));
            
        }
    }
    
    /* Address of the server. */
    
    /****************** Close ****************************************/
    rst = close (sockfd); // Close the socket file descriptor.
    if (rst < 0)
    {
        perror ("Client close problem");
        exit (1);
    }

  }
};


void ToHandleReporter(char *IPAddress,int tcpPort1)
{
   Reporter* reporterObject;
   reporterObject=new Reporter(IPAddress,tcpPort1);
   reporterObject->WriteArticle();
}

void ToHandleReader(char *IPAddress,int tcpPort2)
{
  Reader* readerObject;
  readerObject=new Reader(IPAddress,tcpPort2);
  readerObject->ReadArticle();
}


void ToHandleAdministrator(char *IPAddress,int udpPort1)
{
  Administrator* administratorObject;
  administratorObject=new Administrator(IPAddress,udpPort1);
  administratorObject->newsAdmin();
}

// Call as : ./main <IP_Addr> <TCPport> <UDPport>
int main(int argc,char* argv[])
{
    int tcpPort1,udpPort1,tcpPort2;
    char IPAddress[100];
    
     if (argc < 5) 
     {
         cout << "Error, This is the format <IPAddress> <tcpPort1> <tcpPort2> <udpPort1>\n";
         exit(1);
     }
     else
     {
        strcpy(IPAddress,argv[1]);
        tcpPort1=atoi(argv[2]);
        tcpPort2 =atoi(argv[3]);
        udpPort1=atoi(argv[4]);
     }
     
     
     int choice=0;
     while(choice<1 || choice>3) 
      {
      cout<<"\n*******************************************************************************************************" ;
      cout<<"\n******=>> Press 1 for Reporter access      ..!*********************************************************" ;
      cout<<"\n******=>> Press 2 for Reader access        ..!*********************************************************" ;
      cout<<"\n******=>> Press 3 for Administrator access ..!*********************************************************" ;
      cout<<"\n*******************************************************************************************************\n\n" ;
      
      cout << "Enter the index corresponding the access you desire : " ;
          cin >> choice;
      }
          
     char ch='a';
     while(ch != '#')
     {
        if (choice == 1)
        {
              ToHandleReporter(IPAddress,tcpPort1);
            
        }
        else if(choice == 2)
        {
              ToHandleReader(IPAddress,tcpPort2);
        }
        else if(choice == 3)
        {
             ToHandleAdministrator(IPAddress,udpPort1);
        }
        else
        {
            cout << "Error ! No valid option \n";
            return 0;
        }
        
          cout<<"Please enter # to quit or anything else to continue...\n";
          cin >> ch;
     }
     
    return 0;
}
