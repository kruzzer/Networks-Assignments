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
#include <fstream>
#include <sys/mman.h>
#include <arpa/inet.h>

#define MAX_SIZE 1024
#define MAX_BACKLOG 10
#define MAX_BODY 10000
#define MAX_HEADLINE 500


static int *red_flag;
using namespace std;


void error(const char *msg)
{
    perror(msg);
    exit(1);
}

struct record 
{
    char headline[MAX_HEADLINE];
    time_t dateTime;
    char body[MAX_BODY];
    int type;   // ACADEMIC / NON_ACADEMIC
};


class TcpReaderReporterServer
{
public:


    TcpReaderReporterServer(int port_num,int type)
    {

        sockfd = socket(AF_INET, SOCK_STREAM, 0);

        if(sockfd<0)
        {
            error("Error in opening TCP Server!");
        }

        bzero((char *) &serv_addr, sizeof(serv_addr));
        this->portno = port_num;
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = INADDR_ANY;
        serv_addr.sin_port = htons(port_num);

        if (bind(sockfd,(struct sockaddr *)&serv_addr, sizeof(serv_addr))<0) 
        {
             error("Error in binding the TCP Socket!");
        }
     
        clilen = sizeof(cli_addr);
        clientType=type;
    }
    void waitForRequestPublic()
    {
        this->waitForRequest();
    }
private:
    int newsockfd,clientType;
    pid_t pid;
    int sockfd, portno;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr; // server address and client address respectively

    void error(const char* error_mssg)
    {
        perror(error_mssg);
        exit(1);
    }


    void waitForRequest()
    {
        listen(sockfd,MAX_BACKLOG);
        cout << "...  TCP CONNECTION MADE\n";
        while(1)
        {
            newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

            if (newsockfd < 0)
            {
                cout << "\nError on accept 1!!\n";
            }
            else 
            {
                    char cli_ip[INET_ADDRSTRLEN]; 
                    if(inet_ntop(AF_INET,&(cli_addr.sin_addr),cli_ip,INET_ADDRSTRLEN)==NULL)
                    {
                      error("Error in fixing client IP !");  
                    } 
                    cout<<" TCP Connection established with client " << cli_ip << " !\n\n";
            }
            
            pid=fork();
            if(pid == 0) 
            {
                close(sockfd);
                serverOperation(newsockfd,clientType);
                exit(0);
            }
            else
            {
                close(newsockfd);   
            }
      
        }
    }

    //http://www.eecs.umich.edu/courses/eecs380/HANDOUTS/cppBinaryFileIO-2.html
    void serverOperation(int newsockfd,int clientType)
    {
        int fileCounter = 0,fileReader;
        char *receivedNews;
        receivedNews = (char*)malloc(sizeof(char)*500);
        if(clientType == 1)
        {
            memset (receivedNews, 0,500);
            fileReader = read(newsockfd,receivedNews,500);
            if(fileReader<0)
            {
              error("Error in reading from Reporter  !");  
            }
            //printf("%s\n",receivedNews);
            FILE *file2;
            FILE *file3;
            FILE *file;
            if(strcmp(receivedNews,"/academic") == 0)
            {
                file2 = fopen("news/timelinekeeperacademic.txt", "a");
                file3 = fopen("news/headlinekeeperacademic.txt", "a");
                file =  fopen("news/bodylinekeeperacademic.txt", "a");   
            } 
            else
            {
                file2 = fopen("news/timelinekeepernonacademic.txt", "a");
                file3 = fopen("news/headlinekeepernonacademic.txt", "a");
                file =  fopen("news/bodylinekeepernonacademic.txt", "a");   
            }
            memset (receivedNews, 0,500);
            fileReader = read(newsockfd,receivedNews,500);
            if(fileReader<0)
            {
              error("Error in reading from Reporter  !");  
            }             
            //printf("%s\n",receivedNews);
            time_t dateTime = time(0);
            char *saveThisNews ;
            saveThisNews = (char*)malloc(sizeof(char)*500);
            char *dateT;
            dateT = (char*)malloc(sizeof(char)*500);
            dateT = ctime(&dateTime);
            strcpy(saveThisNews,dateT);
            //saveThisNews=strcat(saveThisNews,"\n");
            
            int results2 = fputs(saveThisNews, file2);
            
            char *receivedNews2;
            receivedNews2 = (char*)malloc(sizeof(char)*500);
            receivedNews2 = strcat(receivedNews,"\n");
            int results3 = fputs(receivedNews2, file3);
            fclose(file2);
            fclose(file3);
            memset (receivedNews, 0,500);
            fileReader = read(newsockfd,receivedNews,500);
            if(fileReader<0)
            {
              error("Error in reading from Reporter  !");  
            }            
            char *receivedNews3; 
            receivedNews3 = (char*)malloc(sizeof(char)*500);
            receivedNews3 = strcat(receivedNews,"\n");
            int results = fputs(receivedNews3, file);
            if (results == EOF) {
             cout << "Error in end of file of reporter"<<endl;   // Failed to write do error code here.
            }
            fclose(file);
            cout<< " TCP Sever has been successful !\n";

        }
        else
        {
            char *typeNameKeeper;
            typeNameKeeper = (char*)malloc(sizeof(char)*500);    
            memset (receivedNews, 0,500);
            fileReader = read(newsockfd,receivedNews,500);
            if(fileReader<0)
            {
              error("Error in reading from Reporter  !");  
            }
            strcpy(typeNameKeeper,receivedNews);
            //printf("%s\n",receivedNews);
            FILE *file2;
            FILE *file3;
            FILE *file;
            if(strcmp(receivedNews,"/academic") == 0)
            {
                file2 = fopen("news/timelinekeeperacademic.txt", "r");
                file3 = fopen("news/headlinekeeperacademic.txt", "r");  
                file =  fopen("news/bodylinekeeperacademic.txt", "r");
            } 
            else
            {
                file2 = fopen("news/timelinekeepernonacademic.txt", "r");
                file3 = fopen("news/headlinekeepernonacademic.txt", "r");
                file =  fopen("news/bodylinekeepernonacademic.txt", "r");
            }

            *red_flag = 1 ;
            int writeChecker;
            memset (receivedNews, 0,500);
            while (fgets( receivedNews, 500, file3))
            {

                writeChecker = write(newsockfd,receivedNews,500);
                if(writeChecker<0)
                {
                    error("Error in sending news!");
                }  
                //printf( "%s\n",receivedNews);
                memset (receivedNews, 0,500);
                fgets( receivedNews, 500, file2);
                writeChecker = write(newsockfd,receivedNews,500);
                if(writeChecker<0)
                {
                    error("Error in sending news!");
                }  
               //printf( "%s\n",receivedNews);
                memset (receivedNews, 0,500);
                fgets(receivedNews, 500, file);
                writeChecker = write(newsockfd,receivedNews,500);
                if(writeChecker<0)
                {
                    error("Error in sending news!");
                }  
                memset(receivedNews, 0,500);
                usleep(100000) ;
            }
            *red_flag = 0 ;
            memset (receivedNews, 0,500);
            
            
        }
    }


};

class udpAdministratorServer
{
public:
    udpAdministratorServer(int port_num)
    {
        sockfd = socket(AF_INET, SOCK_DGRAM, 0);

        if(sockfd<0)
            error("Error in opening UDP ServerS!");

        bzero((char *) &serv_addr, sizeof(serv_addr));
        this->portno = port_num;
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = INADDR_ANY;
        serv_addr.sin_port = htons(port_num);

        if (bind(sockfd,(struct sockaddr *)&serv_addr, sizeof(serv_addr))<0) 
             error("Error in binding the UDP Socket !");
        printf("...  UDP CONNECTION MADE \n");     
        clilen = sizeof(cli_addr);

    }
    void waitForRequestPublic()
    {
        this->handleAdminClient();
    }
private:
    int sockfd, portno;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr; // server address and client address respectively

    void error(const char* error_mssg)
    {
        perror(error_mssg);
        exit(1);
    }


    void handleAdminClient()    
    {

        char *buf;
        buf=(char*)malloc(sizeof(char)*500);
        int udprecv=0;
        int flags=0;

        while(1)
        {
            udprecv=recvfrom(sockfd,buf,500,flags,(struct sockaddr*) &cli_addr,&clilen);
        if(udprecv==-1)
        {
            perror("recvfrom failed");
            exit(1);
        }
        //printf("\nDays Received - %s\n",buf);
        int days=atoi(buf);
        days=days*24*60*60;
        //cout << days << endl;        
        int rst3;
        while(1)
        {    
            char bufferThis[500]="Enter Password\n";
            rst3=sendto(sockfd,bufferThis,500,flags,(struct sockaddr*) &cli_addr,sizeof(struct sockaddr_in));
            if(rst3<0)
            {
                perror("Couldn't Send");
                exit(1);
            }
            flags=0;
            char buf3[500];
            udprecv=recvfrom(sockfd,buf3,500,flags,(struct sockaddr*) &cli_addr,&clilen);
            if(udprecv==-1)
            {
                perror("recvfrom failed");
                exit(1);
            }
            //password received
            if(strcmp(buf3,"12")==0 )
            {
                time_t now = time(0);
                
             if(*red_flag == 0) 
              { 
                strcpy(bufferThis,"Access Granted\n");
                cout << "Deleting Files" << endl ;
                FILE *f1;
                FILE *f2;
                FILE *f3;
                FILE *f4;
                FILE *f5;
                FILE *f6;
                f1 = fopen("news/timelinekeeperacademic.txt", "r");
                f2 = fopen("news/headlinekeeperacademic.txt", "r");  
                f3 = fopen("news/bodylinekeeperacademic.txt", "r");
                f4 = fopen("news/timelinekeepernonacademic.txt", "r");
                f5 = fopen("news/headlinekeepernonacademic.txt", "r");
                f6 = fopen("news/bodylinekeepernonacademic.txt", "r");
                int acadPaper = 0;
                int nonAcadPaper = 0;
                char headLinelistAcademic[50][500];
                char dateLinelistAcademic[50][500];
                char bodyNewslistAcademic[50][500];
                char headLinelistNonAcademic[50][500];
                char dateLinelistNonAcademic[50][500];
                char bodyNewslistNonAcademic[50][500];
                char *itemNews;
                itemNews = (char*)malloc(sizeof(char)*500);
                memset (itemNews, 0,500);
        while (fgets( itemNews, 500, f1))
            {
                //cout << itemNews << endl;
                strcpy(dateLinelistAcademic[acadPaper],itemNews);
                //cout << dateLinelistAcademic[acadPaper] << endl;
                memset (itemNews, 0,500);
                fgets(itemNews, 500, f2);
                strcpy(headLinelistAcademic[acadPaper],itemNews);
                memset (itemNews, 0,500);
                fgets(itemNews, 500, f3);
                strcpy(bodyNewslistAcademic[acadPaper],itemNews);
                memset(itemNews, 0,500);
                acadPaper = acadPaper+1;
            }
            memset(itemNews,0,500);

        while (fgets( itemNews, 500, f4))
            {
                strcpy(dateLinelistNonAcademic[nonAcadPaper],itemNews);
                memset (itemNews, 0,500);
                fgets(itemNews, 500, f5);
                strcpy(headLinelistNonAcademic[nonAcadPaper],itemNews);
                memset (itemNews, 0,500);
                fgets(itemNews, 500, f6);
                strcpy(bodyNewslistNonAcademic[nonAcadPaper],itemNews);
                memset(itemNews, 0,500);
                nonAcadPaper = nonAcadPaper+1;
            }
            memset(itemNews,0,500);
            fclose(f1);
            fclose(f2);
            fclose(f3);
            fclose(f4);
            fclose(f5);
            fclose(f6);
            remove("news/timelinekeeperacademic.txt");
            remove("news/headlinekeeperacademic.txt");  
            remove("news/bodylinekeeperacademic.txt");
            remove("news/timelinekeepernonacademic.txt");
            remove("news/headlinekeepernonacademic.txt");
            remove("news/bodylinekeepernonacademic.txt");
            FILE* ff1 = fopen("news/timelinekeeperacademic.txt", "a");
            FILE* ff2 = fopen("news/headlinekeeperacademic.txt", "a");  
            FILE* ff3 = fopen("news/bodylinekeeperacademic.txt", "a");
            FILE* ff4 = fopen("news/timelinekeepernonacademic.txt", "a");
            FILE* ff5 = fopen("news/headlinekeepernonacademic.txt", "a");
            FILE* ff6 = fopen("news/bodylinekeepernonacademic.txt", "a");
        

            int k;    
            for(k=0;k<nonAcadPaper;k++)
            {
                struct tm tm;
                strptime(dateLinelistNonAcademic[k], "%c", &tm);
                time_t t = mktime(&tm);
               // cout << now << endl;
               // cout << t << endl;
               // cout <<  difftime(now,t) << endl;
               // cout << days << endl;
                if(difftime(now,t) < days) 
                {
                 //   cout << " im in " << endl;
                    fputs(dateLinelistNonAcademic[k], ff4);
                    fputs(headLinelistNonAcademic[k], ff5);
                    fputs(bodyNewslistNonAcademic[k], ff6);
                }
            }
            for(k=0;k<acadPaper;k++)
            {
                struct tm tm;
                strptime(dateLinelistAcademic[k], "%c", &tm);
                time_t t = mktime(&tm);
                //cout <<  difftime(now,t) << endl;
                //cout << now << endl;
                //cout << t << endl;
                if(difftime(now,t) < days) 
                {
                    fputs(dateLinelistAcademic[k], ff1);
                    fputs(headLinelistAcademic[k], ff2);
                    fputs(bodyNewslistAcademic[k], ff3);
                }
                //printf("%s\n",dateLinelistAcademic[k]);   
            }

            fclose(ff1);
            fclose(ff2);
            fclose(ff3);
            fclose(ff4);
            fclose(ff5);
            fclose(ff6);
            
            //printf("\ni wrote here ");
                rst3=sendto(sockfd,bufferThis,500,flags,(struct sockaddr*) &cli_addr,sizeof(struct sockaddr_in));

                if(rst3<0)
                {
                    perror("Couldn't Send");
                    exit(1);
                }
            }
            else 
            {
                strcpy(bufferThis,"Files Could not be deleted as a reader is downloading some files\n"); 
                rst3=sendto(sockfd,bufferThis,500,flags,(struct sockaddr*) &cli_addr,sizeof(struct sockaddr_in));

                if(rst3<0)
                {
                    perror("Couldn't Send");
                    exit(1);
                }         
           }
              break;
            }


            
        }    
        }
        
        

        //seconds = difftime(now,mktime(&newyear));

    }
};






int main(int argc,char* argv[])
{

    red_flag =(int *) mmap(NULL, sizeof *red_flag, PROT_READ | PROT_WRITE, 
                    MAP_SHARED | MAP_ANONYMOUS, -1, 0); // this is for synchronisation

    *red_flag = 0 ;
    int tcpPort1,tcpPort2,udpPort1;

    if (argc != 4) 
    {
         cout << "No ports have been provide ...\nConnection failed ...\n";
         exit(1);
     }
     else
     {
        tcpPort1=atoi(argv[1]);
        tcpPort2=atoi(argv[2]);
        udpPort1=atoi(argv[3]);
     }

     pid_t pid1,pid2,pid3;

     pid1=fork();

     if(pid1==0)
     {

     }
     else
     {
        pid2=fork();   
     }
     
    if(pid1!=0 && pid2!=0)
    {
        pid3=fork();
   
    }
     

     if(pid1<0 || pid2<0 || pid3<0)
     {
        cout << "Fork Error \n";
        exit(1);
     }
     else if(pid1==0)
     {
        TcpReaderReporterServer tcpServerReporter = TcpReaderReporterServer(tcpPort1,1);  // for Reporter Client
        tcpServerReporter.waitForRequestPublic();
     }
     else if(pid2==0)
     {
        TcpReaderReporterServer tcpServerReader = TcpReaderReporterServer(tcpPort2,0);    // for Reader Client
        tcpServerReader.waitForRequestPublic();
     }
     else if (pid3==0)
     {
        
        udpAdministratorServer udpServerAdmin = udpAdministratorServer(udpPort1); // for Administrative client
        udpServerAdmin.waitForRequestPublic();   
            
     }
     else
     {
        wait(NULL);
        wait(NULL);
        wait(NULL);
     }


    return 0;
}
