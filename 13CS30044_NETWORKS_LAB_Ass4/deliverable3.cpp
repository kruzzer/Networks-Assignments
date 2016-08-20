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


int main(int argc , char * argv[] )
{
  while(1)
  {
      char multiple[10];
      cout << "Do you have multiple accounts? press Y for YES and N for NO " << endl;
      cin >> multiple;
      if(strcmp(multiple,"Y") == 0)
      {
        char ipAddress1[50];
        char ipAddress2[50];
        char port1[50];
        char port2[50];
        cout << "IP Address of the 1st connection:" << endl;
        cin >> ipAddress1;
        cout << "Port on 1st IP Address:" << endl;
        cin >> port1;
        cout << "IP Address of the 2nd connection:" << endl;
        cin >> ipAddress2;
        cout << "Port on 2nd IP Address:" << endl;
        cin >> port2;
        pid_t pid1 = fork();
        if(pid1 == 0)
        {
          execlp("xterm","CLIENT1","-hold","-e","./client",ipAddress1,port1,NULL) ;
        }
        else    
        {
          pid_t pid2 = fork();
          if(pid2 == 0)
          {
              execlp("xterm","CLIENT2","-hold","-e","./client",ipAddress2,port2,NULL) ;    
          }
        }
        break;
      }
      else if(strcmp(multiple,"N") == 0)
      {
        char ipAddress1[50];
        char port1[50];
        cout << "IP Address of the 1st connection:" << endl;
        cin >> ipAddress1;
        cout << "Port on 1st IP Address:" << endl;
        cin >> port1;

        execlp("xterm","xterm","-hold","-e","./client",ipAddress1,port1,NULL) ;

        break;
      }
      else
      {
        cout << "Wrong input !" << endl;
        cout << "Asking again ! Do you have multiple accounts? press Y for YES and N for NO " << endl;
        cin >> multiple;
      }

  }
  wait(NULL);
  wait(NULL);
  return 0;
}
