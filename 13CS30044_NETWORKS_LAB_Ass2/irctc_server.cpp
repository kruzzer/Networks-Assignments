#include <bits/stdc++.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
#include <dirent.h>
#include <string.h>
#include <sys/mman.h>
#include <errno.h>
#include <signal.h>

using namespace std ;

int port_number = 5556 ;

static int *avail_raj , *avail_ac , *avail_sleeper ;


class booking 
{
  public : 
  int id ;
  int train_no ;
  int type ; // 1 for ac and 2 for sleeper
  int no_of_booking ;
  int no_of_senior ;
  int age[100] ;
  char preferences[100][3] ;
};

booking parser(char * s)
{
    int state = 0 ;
    int l = strlen(s) ;
    int k = 0 , j = 0 , t = 0 ;
    booking tuple  ;
    char temp[6][100] ;
    char AGE[3] ;
 
    for(int i = 0 ; i < l ; i++)
    {
    //   cout << s[i] << endl ;
       if(s[i] == ',')
          {
            temp[k][j] = '\0' ;
            k++ ;
            j=0 ;
          }
          else
          temp[k][j++] = s[i]   ;   
    }
    temp[k][j] = '\0' ;
    
    //for(int i = 0 ; i < 6 ; i++)
      // cout << temp[i] << endl ; 
    
    tuple.id = atoi(temp[0]) ;
    tuple.train_no = atoi(temp[1]) ;
    if(temp[2][0] == 'A')
      tuple.type = 1 ;
    else 
     tuple.type = 2  ;

   tuple.no_of_booking  = atoi(temp[3]); 
   

   l = strlen(temp[4]) ;
    k = 0 ; j = 0 ;

   for(int i = 0 ; i < l ; i++)
   {
     if(temp[4][i] != '-')
       AGE[k++] = temp[4][i] ;
     else
     {
      AGE[k] = '\0' ;
      strcpy(tuple.preferences[j++],AGE) ;
      k = 0 ;
     }  
   }

    AGE[k] = '\0' ; 
    strcpy(tuple.preferences[j++],AGE) ;
 
   
   l = strlen(temp[5]) ;
    k = 0 ; j = 0 ;

   for(int i = 0 ; i < l ; i++)
   {
     if(temp[5][i] != '-')
       AGE[k++] = temp[5][i] ;
     else
     {
      AGE[k] = '\0' ;
      tuple.age[j++] = atoi(AGE) ;
      k = 0 ;
     }  
   }

    AGE[k] = '\0' ; 
    tuple.age[j++] = atoi(AGE) ;

     tuple.no_of_senior = 0 ; 
    for(int i = 0 ; i < tuple.no_of_booking ; i ++)
    {
       if(tuple.age[i] > 60)
        tuple.no_of_senior ++ ;
    }

    return tuple ;
}

class train_ac_rajdhani
{
  public : 
  int no_of_seats_available ;
  
  int no_of_seats_coach[13] ;

  int no_of_lower_births[13] ;
  int no_of_upper_births[13] ;
  //int no_of_middle_births[13] ;
  int no_of_side_upper[13] ;
  int no_of_side_lower[13] ;
    char available[13][54] ;
 
   train_ac_rajdhani()
  {
    no_of_seats_available = 702 ;

      
      for(int i = 0 ; i < 13 ; i++)
     {
        no_of_seats_coach[i] = 54 ;
        no_of_lower_births[i] = 18 ;
        no_of_upper_births[i] = 18 ;
          //no_of_middle_births[13] ;
        no_of_side_upper[i] = 9  ;
        no_of_side_lower[i] = 9 ;

        for(int j = 0 ; j < 54 ; j++)
        available[i][j] = 'Y' ;
     }
  }

    int book_function(booking tuple , char *s)
  {
       
        //cout << "AVAILABLE SEATS : " << no_of_seats_available << endl ;
       if(tuple.no_of_booking > no_of_seats_available)
       {
          strcpy(s,"Sorry ! No births available") ;
          return -1 ;
       }
       
       int ub = 0 , lb = 0 , su = 0, sl = 0 ;
       
       int j ; 

       int flag = 0 ;

       for(int i = 0 ; i < tuple.no_of_booking ; i++)
        {
          if(!strcmp(tuple.preferences[i],"NA"))
          {
            flag = 1 ;
            break ; 
          }
           if(!strcmp(tuple.preferences[i],"SU"))
              su++ ;
           else if(!strcmp(tuple.preferences[i],"SL"))
              sl++ ;
           else if(!strcmp(tuple.preferences[i],"UB"))
              ub++ ;
           else if(!strcmp(tuple.preferences[i],"LB"))
              lb++ ;            
       
        }

        //cout << lb << endl ;
            //cout << "CHECK : " << available[1][1] << endl ;
           // for(;;) ;
        for(int i = 0 ; i < 13 && flag == 0 ; i++)
        {
          if(no_of_seats_coach[i] < tuple.no_of_booking)
             continue ;
          if(
                   no_of_lower_births[i] >= lb &&
                   no_of_upper_births[i] >= ub &&
                   no_of_side_upper[i]   >= su &&
                   no_of_side_lower[i]   >= sl 
                  )
            {
                //cout  << "  YO CORRECT  "  ;
             
                int seats[100] , k = 0 ;
                                    
                   for(j = 1 ; j < 54 && ub > 0 ; j+=6)
                   {
                      if(available[i][j] == 'Y' && ub > 0)
                        {
                          seats[k++] = j+1 ;
                          available[i][j] = 'N' ;
                       //   cout << "in ub available[" << i+1<< "][" << j+1 << "] = " << available[i][j] << endl ;
                          ub -- ;
                          no_of_upper_births[i]-- ;
                          no_of_seats_coach[i]-- ;
                          no_of_seats_available-- ;
                        }
                      if(available[i][j+2] == 'Y' && ub > 0)
                        {
                          seats[k++] = j+3 ;
                          available[i][j+2] = 'N' ;
                      //    cout << "in ub available[" << i+1<< "][" << j+1 << "] = " << available[i][j] << endl ;
                          ub-- ;
                          no_of_upper_births[i]-- ;
                          no_of_seats_coach[i]-- ;
                          no_of_seats_available-- ;
                        }  

                   }   
                

                 
                   for(j = 0 ; j < 54 && lb > 0 ; j+=6)
                   {  
                      //   cout << "i = " << i+1 << "j = " << j+1 <<"\t" ;
                      if(available[i][j] == 'Y' && lb > 0)
                        {

                          seats[k++] = j+1 ;
                          available[i][j] = 'N' ;
                       //   cout << "in lb1 available[" << i+1<< "][" << j+1 << "] = " << available[i][j] << endl ;
                          lb -- ;
                          no_of_lower_births[i]-- ;
                          no_of_seats_coach[i]-- ;
                          no_of_seats_available-- ;
                        }
                      if(available[i][j+2] == 'Y'&& lb > 0)
                        {
                          seats[k++] = j+3 ;
                          available[i][j+2] = 'N' ;
                        // cout << "in lb2 available[" << i+1<< "][" << j+3 << "] = " << available[i][j] << endl ;
                          lb-- ;
                          no_of_lower_births[i]-- ;
                          no_of_seats_coach[i]-- ;
                          no_of_seats_available-- ;
                        }  

                   }   
                

                
                   for(j = 5 ; j < 54 && su > 0 ; j+=6)
                   {
                      if(available[i][j] == 'Y' && su > 0)
                        {
                          seats[k++] = j+1 ;
                          available[i][j] = 'N' ;
                          //cout << "in su available[" << i+1<< "][" << j+1 << "] = " << available[i][j] << endl ;
                          su -- ;
                          no_of_side_upper[i]-- ;
                          no_of_seats_coach[i]-- ;
                          no_of_seats_available-- ;
                        } 

                   }   

                   for(j = 4 ; j < 54 && sl > 0 ; j+=6)
                   {
                      if(available[i][j] == 'Y' && sl > 0)
                        {
                          seats[k++] = j+1 ;
                          available[i][j] = 'N' ;
                          //cout << "in sl available[" << i+1<< "][" << j+1 << "] = " << available[i][j] << endl ;
                          sl -- ;
                          no_of_side_lower[i]-- ;
                          no_of_seats_coach[i]-- ;
                          no_of_seats_available-- ;
                        }
                   }   
            
             
              char ticket[100] ;
              sprintf(ticket,"ID : %d  CNF Coach : A-%d  Seats : " , tuple.id , i+1) ;
              strcat(s,ticket) ;
              for(j = 0 ; j < k-1 ; j++)
              {
                 sprintf(ticket," %d" , seats[j]) ;
                 strcat(s,ticket) ;
                 if((seats[j]-1)%6 == 1 || (seats[j]-1)%6 == 3)
                    sprintf(ticket,"(UB),") ;

                 if((seats[j]-1)%6 == 0 || (seats[j]-1)%6 == 2)
                    sprintf(ticket,"(LB),") ;

                 if((seats[j]-1)%6 == 5)
                    sprintf(ticket,"(SU),") ;

                 if((seats[j]-1)%6 == 4)
                    sprintf(ticket,"(SL),") ;

                 strcat(s,ticket) ; 

              }
                 sprintf(ticket," %d" , seats[j]) ;
                 strcat(s,ticket) ;

                 if((seats[j]-1)%6 == 1 || (seats[j]-1)%6 == 3)
                    sprintf(ticket,"(UB)") ;

                 if((seats[j]-1)%6 == 0 || (seats[j]-1)%6 == 2)
                    sprintf(ticket,"(LB)") ;

                 if((seats[j]-1)%6 == 5)
                    sprintf(ticket,"(SU)") ;

                 if((seats[j]-1)%6 == 4)
                    sprintf(ticket,"(SL)") ;

                 strcat(s,ticket) ;
                 return 1 ;
            }
        } 

    


        for( int i = 0 ; i < 13 ; i++)
        {
           if(no_of_seats_coach[i] >= tuple.no_of_booking)
           {
            //cout  << "  YO REAL  "  ;
              int seats[100] ; 
              int k = 0 ;
              int t = tuple.no_of_booking ;
              for(j = 0 ; j < 54 && t > 0 ; j++)
                {
                  if(available[i][j] == 'Y')
                  { 
                      seats[k++] = j+1 ; 
                      available[i][j] = 'N' ;
                     
                      if(j%6 == 1 || j%6 == 3)
                        no_of_upper_births[i]-- ;

                     if(j%6 == 0 || j%6 == 2)
                        no_of_lower_births[i]-- ;

                     if(j%6 == 5)
                        no_of_side_upper[i]-- ;

                     if(j%6 == 4)
                        no_of_side_lower[i]-- ; 

                    no_of_seats_coach[i]-- ;
                    no_of_seats_available-- ; 
                    t-- ;                
                  }
                }

              char ticket[100] ;
              sprintf(ticket,"ID : %d  CNF Coach : A-%d  Seats : " , tuple.id , i +1 ) ;
              strcat(s,ticket) ;
              for(j = 0 ; j < k-1 ; j++)
              {
                 sprintf(ticket," %d," , seats[j]) ;
                 strcat(s,ticket) ;
                 if((seats[j]-1)%6 == 1 || (seats[j]-1)%6 == 3)
                    sprintf(ticket,"(UB),") ;

                 if((seats[j]-1)%6 == 0 || (seats[j]-1)%6 == 2)
                    sprintf(ticket,"(LB),") ;

                 if((seats[j]-1)%6 == 5)
                    sprintf(ticket,"(SU),") ;

                 if((seats[j]-1)%6 == 4)
                    sprintf(ticket,"(SL),") ;

                 strcat(s,ticket) ;
              }
                 sprintf(ticket," %d" , seats[j]) ;
                 strcat(s,ticket) ;
                 if((seats[j]-1)%6 == 1 || (seats[j]-1)%6 == 3)
                    sprintf(ticket,"(UB)") ;

                 if((seats[j]-1)%6 == 0 || (seats[j]-1)%6 == 2)
                    sprintf(ticket,"(LB)") ;

                 if((seats[j]-1)%6 == 5)
                    sprintf(ticket,"(SU)") ;

                 if((seats[j]-1)%6 == 4)
                    sprintf(ticket,"(SL)") ;

                 strcat(s,ticket) ;
                 return 1 ;


           }
        }
      char tickets[100]; 
        sprintf(tickets,"ID : %d  CNF " , tuple.id ) ;
        strcat(s,tickets) ;

        int t = tuple.no_of_booking ;

        for( int i = 0 ; i < 13 && t > 0; i++)
        {


          if(no_of_seats_coach[i] > 0 && t > 0)
          { 
              //cout  << "  YO  " << endl ;
              int seats[100] , k = 0 ;
             for(j = 0 ; j < 54 && no_of_seats_coach[i] > 0 && t > 0; j++)
             {
               if(available[i][j] == 'Y')
                  { 
                    seats[k++] = j+1 ; 
                    available[i][j] = 'N' ;
                   
                    if(j%6 == 1 || j%6 == 3)
                        no_of_upper_births[i]-- ;

                     if(j%6 == 0 || j%6 == 2)
                        no_of_lower_births[i]-- ;

                     if(j%6 == 5)
                        no_of_side_upper[i]-- ;

                     if(j%6 == 4)
                        no_of_side_lower[i]-- ; 

                    no_of_seats_coach[i]-- ;
                    no_of_seats_available-- ; 
                    t-- ;    

                  }
               
             }

              char ticket[100] ;
              sprintf( ticket , " Coach : A-%d  Seats : " , i+1 ) ;
              strcat(s,ticket) ;
              for(j = 0 ; j < k-1 ; j++)
              {
                 sprintf(ticket," %d" , seats[j]) ;
                 strcat(s,ticket) ;
                 if((seats[j]-1)%6 == 1 || (seats[j]-1)%6 == 3)
                    sprintf(ticket,"(UB),") ;

                 if((seats[j]-1)%6 == 0 || (seats[j]-1)%6 == 2)
                    sprintf(ticket,"(LB),") ;

                 if((seats[j]-1)%6 == 5)
                    sprintf(ticket,"(SU),") ;

                 if((seats[j]-1)%6 == 4)
                    sprintf(ticket,"(SL),") ;

                 strcat(s,ticket) ;
              }

              sprintf(ticket," %d" , seats[j]) ;
                 strcat(s,ticket) ;
              if((seats[j]-1)%6 == 1 || (seats[j]-1)%6 == 3)
                    sprintf(ticket,"(UB)") ;

                 if((seats[j]-1)%6 == 0 || (seats[j]-1)%6 == 2)
                    sprintf(ticket,"(LB)") ;

                 if((seats[j]-1)%6 == 5)
                    sprintf(ticket,"(SU)") ;

                 if((seats[j]-1)%6 == 4)
                    sprintf(ticket,"(SL)") ;

                 strcat(s,ticket) ;   

          }

        }

        return 1 ;

  }

} ;

// booking rajdhani finished
class train_ac_superfast
{
  public : 
  int no_of_seats_available ;
  
  int no_of_seats_coach[3] ;

  int no_of_lower_births[3] ;
  int no_of_upper_births[3] ;
  int no_of_middle_births[3] ;
  int no_of_side_upper[3] ;
  int no_of_side_lower[3] ;
  char available[3][72] ;

   train_ac_superfast()
  {
    no_of_seats_available = 216 ;

      
      for(int i = 0 ; i < 3 ; i++)
     {
        no_of_seats_coach[i] = 72 ;
        no_of_lower_births[i] = 18 ;
        no_of_upper_births[i] = 18 ;
        no_of_middle_births[i] = 18;
        no_of_side_upper[i] = 9 ;
        no_of_side_lower[i] = 9 ;

        for(int j = 0 ; j < 72 ; j++)
        available[i][j] = 'Y' ;
     }
  }

    int book_function(booking tuple , char *s)
  {
       
        //cout << "AVAILABLE SEATS : " << no_of_seats_available << endl ;
       if(tuple.no_of_booking > no_of_seats_available)
       {
          strcpy(s,"Sorry ! No births available") ;
          return -1 ;
       }
       
       int ub = 0 , lb = 0 , su = 0, sl = 0 , mb = 0 ;
       
       int j ; 

       int flag = 0 ;

       for(int i = 0 ; i < tuple.no_of_booking ; i++)
        {
          if(!strcmp(tuple.preferences[i],"NA"))
          {
            flag = 1 ;
            break ; 
          }
           if(!strcmp(tuple.preferences[i],"SU"))
              su++ ;
           else if(!strcmp(tuple.preferences[i],"SL"))
              sl++ ;
           else if(!strcmp(tuple.preferences[i],"UB"))
              ub++ ;
           else if(!strcmp(tuple.preferences[i],"LB"))
              lb++ ;
           else if(!strcmp(tuple.preferences[i],"MB"))
              su++ ;               
        }

        //cout << lb << endl ;
            //cout << "CHECK : " << available[1][1] << endl ;
           // for(;;) ;
        for(int i = 0 ; i < 3 && flag == 0 ; i++)
        {
          if(no_of_seats_coach[i] < tuple.no_of_booking)
             continue ;
          if(
                   no_of_lower_births[i] >= lb &&
                   no_of_upper_births[i] >= ub &&
                   no_of_side_upper[i]   >= su &&
                   no_of_side_lower[i]   >= sl &&
                   no_of_middle_births[i]>= mb
                  )
            {
                //cout  << "  YO CORRECT  "  ;
             
                int seats[100] , k = 0 ;
                                    
                   for(j = 2 ; j < 72 && ub > 0 ; j+=8)
                   {
                      if(available[i][j] == 'Y' && ub > 0)
                        {
                          seats[k++] = j+1 ;
                          available[i][j] = 'N' ;
                       //   cout << "in ub available[" << i+1<< "][" << j+1 << "] = " << available[i][j] << endl ;
                          ub -- ;
                          no_of_upper_births[i]-- ;
                          no_of_seats_coach[i]-- ;
                          no_of_seats_available-- ;
                        }
                      if(available[i][j+3] == 'Y' && ub > 0)
                        {
                          seats[k++] = j+4 ;
                          available[i][j+3] = 'N' ;
                      //    cout << "in ub available[" << i+1<< "][" << j+1 << "] = " << available[i][j] << endl ;
                          ub-- ;
                          no_of_upper_births[i]-- ;
                          no_of_seats_coach[i]-- ;
                          no_of_seats_available-- ;
                        }  

                   }   
                

                 
                   for(j = 0 ; j < 72 && lb > 0 ; j+=8)
                   {  
                      //   cout << "i = " << i+1 << "j = " << j+1 <<"\t" ;
                      if(available[i][j] == 'Y' && lb > 0)
                        {

                          seats[k++] = j+1 ;
                          available[i][j] = 'N' ;
                       //   cout << "in lb1 available[" << i+1<< "][" << j+1 << "] = " << available[i][j] << endl ;
                          lb -- ;
                          no_of_lower_births[i]-- ;
                          no_of_seats_coach[i]-- ;
                          no_of_seats_available-- ;
                        }
                      if(available[i][j+3] == 'Y'&& lb > 0)
                        {
                          seats[k++] = j+4 ;
                          available[i][j+3] = 'N' ;
                        // cout << "in lb2 available[" << i+1<< "][" << j+3 << "] = " << available[i][j] << endl ;
                          lb-- ;
                          no_of_lower_births[i]-- ;
                          no_of_seats_coach[i]-- ;
                          no_of_seats_available-- ;
                        }  

                   }

                   for(j = 1 ; j < 72 && mb > 0 ; j+=8)
                   {  
                      //   cout << "i = " << i+1 << "j = " << j+1 <<"\t" ;
                      if(available[i][j] == 'Y' && mb > 0)
                        {

                          seats[k++] = j+1 ;
                          available[i][j] = 'N' ;
                       //   cout << "in lb1 available[" << i+1<< "][" << j+1 << "] = " << available[i][j] << endl ;
                          mb -- ;
                          no_of_middle_births[i]-- ;
                          no_of_seats_coach[i]-- ;
                          no_of_seats_available-- ;
                        }
                      if(available[i][j+3] == 'Y'&& mb > 0)
                        {
                          seats[k++] = j+4 ;
                          available[i][j+3] = 'N' ;
                        // cout << "in lb2 available[" << i+1<< "][" << j+3 << "] = " << available[i][j] << endl ;
                          mb-- ;
                          no_of_middle_births[i]-- ;
                          no_of_seats_coach[i]-- ;
                          no_of_seats_available-- ;
                        }  

                   }   
                

                
                   for(j = 7 ; j < 72 && su > 0 ; j+=8)
                   {
                      if(available[i][j] == 'Y' && su > 0)
                        {
                          seats[k++] = j+1 ;
                          available[i][j] = 'N' ;
                          //cout << "in su available[" << i+1<< "][" << j+1 << "] = " << available[i][j] << endl ;
                          su -- ;
                          no_of_side_upper[i]-- ;
                          no_of_seats_coach[i]-- ;
                          no_of_seats_available-- ;
                        } 

                   }   

                   for(j = 6 ; j < 72 && sl > 0 ; j+=8)
                   {
                      if(available[i][j] == 'Y' && sl > 0)
                        {
                          seats[k++] = j+1 ;
                          available[i][j] = 'N' ;
                          //cout << "in sl available[" << i+1<< "][" << j+1 << "] = " << available[i][j] << endl ;
                          sl -- ;
                          no_of_side_lower[i]-- ;
                          no_of_seats_coach[i]-- ;
                          no_of_seats_available-- ;
                        }
                   }   
            
             
              char ticket[100] ;
              sprintf(ticket,"ID : %d  CNF Coach : A-%d  Seats : " , tuple.id , i+1) ;
              strcat(s,ticket) ;
              for(j = 0 ; j < k-1 ; j++)
              {
                 sprintf(ticket," %d" , seats[j]) ;
                 strcat(s,ticket) ;
                 if((seats[j]-1)%8 == 2 || (seats[j]-1)%8 == 5)
                       sprintf(ticket,"(UB),") ;

                     if((seats[j]-1)%8 == 0 || (seats[j]-1)%8 == 3)
                       sprintf(ticket,"(LB),") ;

                     if((seats[j]-1)%8 == 7)
                        sprintf(ticket,"(SU),") ;

                     if((seats[j]-1)%8 == 6)
                        sprintf(ticket,"(SL),") ; 
                     
                    if((seats[j]-1)%8 == 1 || (seats[j]-1)%8 == 4)
                       sprintf(ticket,"(MB),") ;

                 strcat(s,ticket) ;
              }
                 sprintf(ticket," %d" , seats[j]) ;
                 strcat(s,ticket) ;
                 if((seats[j]-1)%8 == 2 || (seats[j]-1)%8 == 5)
                       sprintf(ticket,"(UB)") ;

                     if((seats[j]-1)%8 == 0 || (seats[j]-1)%8 == 3)
                       sprintf(ticket,"(LB)") ;

                     if((seats[j]-1)%8 == 7)
                        sprintf(ticket,"(SU)") ;

                     if((seats[j]-1)%8 == 6)
                        sprintf(ticket,"(SL)") ; 
                     
                    if((seats[j]-1)%8 == 1 || (seats[j]-1)%8 == 4)
                       sprintf(ticket,"(MB)") ;

                 strcat(s,ticket) ;
                 return 1 ;
            }
        } 

    


        for( int i = 0 ; i < 3 ; i++)
        {
           if(no_of_seats_coach[i] >= tuple.no_of_booking)
           {
            //cout  << "  YO REAL  "  ;
              int seats[100] ; 
              int k = 0 ;
              int t = tuple.no_of_booking ;
              for(j = 0 ; j < 72 && t > 0 ; j++)
                {
                  if(available[i][j] == 'Y')
                  { 
                      seats[k++] = j+1 ; 
                      available[i][j] = 'N' ;
                     
                      if(j%8 == 2 || j%8 == 5)
                        no_of_upper_births[i]-- ;

                     if(j%8 ==  0 || j%8 == 3)
                        no_of_lower_births[i]-- ;

                     if(j%8 == 7)
                        no_of_side_upper[i]-- ;

                     if(j%8 == 6)
                        no_of_side_lower[i]-- ; 
                     
                    if(j%8 == 1 || j%8 == 4)
                       no_of_middle_births[i]-- ;     

                    no_of_seats_coach[i]-- ;
                    no_of_seats_available-- ; 
                    t-- ;                
                  }
                }

              char ticket[100] ;
              sprintf(ticket,"ID : %d  CNF Coach : A-%d  Seats : " , tuple.id , i +1 ) ;
              strcat(s,ticket) ;
              for(j = 0 ; j < k-1 ; j++)
              {
                 sprintf(ticket," %d" , seats[j]) ;
                 strcat(s,ticket) ;
                 
                 if((seats[j]-1)%8 == 2 || (seats[j]-1)%8 == 5)
                       sprintf(ticket,"(UB),") ;

                     if((seats[j]-1)%8 == 0 || (seats[j]-1)%8 == 3)
                       sprintf(ticket,"(LB),") ;

                     if((seats[j]-1)%8 == 7)
                        sprintf(ticket,"(SU),") ;

                     if((seats[j]-1)%8 == 6)
                        sprintf(ticket,"(SL),") ; 
                     
                    if((seats[j]-1)%8 == 1 || (seats[j]-1)%8 == 4)
                       sprintf(ticket,"(MB),") ;
                 strcat(s,ticket) ;
              }
                 sprintf(ticket," %d" , seats[j]) ;
                 strcat(s,ticket) ;
                 if((seats[j]-1)%8 == 2 || (seats[j]-1)%8 == 5)
                       sprintf(ticket,"(UB)") ;

                     if((seats[j]-1)%8 == 0 || (seats[j]-1)%8 == 3)
                       sprintf(ticket,"(LB)") ;

                     if((seats[j]-1)%8 == 7)
                        sprintf(ticket,"(SU)") ;

                     if((seats[j]-1)%8 == 6)
                        sprintf(ticket,"(SL)") ; 
                     
                    if((seats[j]-1)%8 == 1 || (seats[j]-1)%8 == 4)
                       sprintf(ticket,"(MB)") ;
                 strcat(s,ticket) ;
                 return 1 ;


           }
        }
      char tickets[100]; 
        sprintf(tickets,"ID : %d  CNF " , tuple.id ) ;
        strcat(s,tickets) ;

        int t = tuple.no_of_booking ;

        for( int i = 0 ; i < 3 && t > 0; i++)
        {


          if(no_of_seats_coach[i] > 0 && t > 0)
          { 
              //cout  << "  YO  " << endl ;
              int seats[100] , k = 0 ;
             for(j = 0 ; j < 72 && no_of_seats_coach[i] > 0 && t > 0; j++)
             {
               if(available[i][j] == 'Y')
                  { 
                    seats[k++] = j+1 ; 
                    available[i][j] = 'N' ;
                   
                    if(j%8 == 2 || j%8 == 5)
                        no_of_upper_births[i]-- ;

                     if(j%8 == 0 || j%8 == 3)
                        no_of_lower_births[i]-- ;

                     if(j%8 == 7)
                        no_of_side_upper[i]-- ;

                     if(j%8 == 6)
                        no_of_side_lower[i]-- ; 
                     
                    if(j%8 == 1 || j%8 == 4)
                       no_of_middle_births[i]-- ; 

                    no_of_seats_coach[i]-- ;
                    no_of_seats_available-- ; 
                    t-- ;    

                  }
               
             }

              char ticket[100] ;
              sprintf( ticket , " Coach : A-%d  Seats : " , i+1 ) ;
              strcat(s,ticket) ;
              for(j = 0 ; j < k-1 ; j++)
              {
                 sprintf(ticket," %d" , seats[j]) ;
                 strcat(s,ticket) ;
                 if((seats[j]-1)%8 == 2 || (seats[j]-1)%8 == 5)
                       sprintf(ticket,"(UB),") ;

                     if((seats[j]-1)%8 == 0 || (seats[j]-1)%8 == 3)
                       sprintf(ticket,"(LB),") ;

                     if((seats[j]-1)%8 == 7)
                        sprintf(ticket,"(SU),") ;

                     if((seats[j]-1)%8 == 6)
                        sprintf(ticket,"(SL),") ; 
                     
                    if((seats[j]-1)%8 == 1 || (seats[j]-1)%8 == 4)
                       sprintf(ticket,"(MB),") ;
                 strcat(s,ticket) ;
              }

              sprintf(ticket," %d" , seats[j]) ;
                 strcat(s,ticket) ;
                 if((seats[j]-1)%8 == 2 || (seats[j]-1)%8 == 5)
                       sprintf(ticket,"(UB)") ;

                     if((seats[j]-1)%8 == 0 || (seats[j]-1)%8 == 3)
                       sprintf(ticket,"(LB)") ;

                     if((seats[j]-1)%8 == 7)
                        sprintf(ticket,"(SU)") ;

                     if((seats[j]-1)%8 == 6)
                        sprintf(ticket,"(SL)") ; 
                     
                    if((seats[j]-1)%8 == 1 || (seats[j]-1)%8 == 4)
                       sprintf(ticket,"(MB)") ;;
                 strcat(s,ticket) ;

          }

        }

        return 1 ;

  }

} ;


class train_sleeper_superfast
{
  public : 
  int no_of_seats_available ;
  
  int no_of_seats_coach[12] ;

  int no_of_lower_births[12] ;
  int no_of_upper_births[12] ;
  int no_of_middle_births[12] ;
  int no_of_side_upper[12] ;
  int no_of_side_lower[12] ;
    char available[12][72] ;
 
   train_sleeper_superfast()
  {
    no_of_seats_available = 864 ;

      
      for(int i = 0 ; i < 12 ; i++)
     {
        no_of_seats_coach[i] = 72 ;
        no_of_lower_births[i] = 18 ;
        no_of_upper_births[i] = 18 ;
        no_of_middle_births[i] = 18;
        no_of_side_upper[i] = 9 ;
        no_of_side_lower[i] = 9 ;

        for(int j = 0 ; j < 72 ; j++)
        available[i][j] = 'Y' ;
     }
  }

    int book_function(booking tuple , char *s)
  {
       
       // cout << "AVAILABLE SEATS : " << no_of_seats_available << endl ;
       if(tuple.no_of_booking > no_of_seats_available)
       {
          strcpy(s,"Sorry ! No births available") ;
          return -1 ;
       }
       
       int ub = 0 , lb = 0 , su = 0, sl = 0 , mb = 0 ;
       
       int j ; 

       int flag = 0 ;

       for(int i = 0 ; i < tuple.no_of_booking ; i++)
        {
          if(!strcmp(tuple.preferences[i],"NA"))
          {
            flag = 1 ;
            break ; 
          }
           if(!strcmp(tuple.preferences[i],"SU"))
              su++ ;
           else if(!strcmp(tuple.preferences[i],"SL"))
              sl++ ;
           else if(!strcmp(tuple.preferences[i],"UB"))
              ub++ ;
           else if(!strcmp(tuple.preferences[i],"LB"))
              lb++ ;
           else if(!strcmp(tuple.preferences[i],"MB"))
              su++ ;               
        }

        //cout << lb << endl ;
            //cout << "CHECK : " << available[1][1] << endl ;
           // for(;;) ;
        for(int i = 0 ; i < 12 && flag == 0 ; i++)
        {
          if(no_of_seats_coach[i] < tuple.no_of_booking)
             continue ;
          if(
                   no_of_lower_births[i] >= lb &&
                   no_of_upper_births[i] >= ub &&
                   no_of_side_upper[i]   >= su &&
                   no_of_side_lower[i]   >= sl &&
                   no_of_middle_births[i]>= mb
                  )
            {
                //cout  << "  YO CORRECT  "  ;
             
                int seats[100] , k = 0 ;
                                    
                   for(j = 2 ; j < 72 && ub > 0 ; j+=8)
                   {
                      if(available[i][j] == 'Y' && ub > 0)
                        {
                          seats[k++] = j+1 ;
                          available[i][j] = 'N' ;
                       //   cout << "in ub available[" << i+1<< "][" << j+1 << "] = " << available[i][j] << endl ;
                          ub -- ;
                          no_of_upper_births[i]-- ;
                          no_of_seats_coach[i]-- ;
                          no_of_seats_available-- ;
                        }
                      if(available[i][j+3] == 'Y' && ub > 0)
                        {
                          seats[k++] = j+4 ;
                          available[i][j+3] = 'N' ;
                      //    cout << "in ub available[" << i+1<< "][" << j+1 << "] = " << available[i][j] << endl ;
                          ub-- ;
                          no_of_upper_births[i]-- ;
                          no_of_seats_coach[i]-- ;
                          no_of_seats_available-- ;
                        }  

                   }   
                

                 
                   for(j = 0 ; j < 72 && lb > 0 ; j+=8)
                   {  
                      //   cout << "i = " << i+1 << "j = " << j+1 <<"\t" ;
                      if(available[i][j] == 'Y' && lb > 0)
                        {

                          seats[k++] = j+1 ;
                          available[i][j] = 'N' ;
                       //   cout << "in lb1 available[" << i+1<< "][" << j+1 << "] = " << available[i][j] << endl ;
                          lb -- ;
                          no_of_lower_births[i]-- ;
                          no_of_seats_coach[i]-- ;
                          no_of_seats_available-- ;
                        }
                      if(available[i][j+3] == 'Y'&& lb > 0)
                        {
                          seats[k++] = j+4 ;
                          available[i][j+3] = 'N' ;
                        // cout << "in lb2 available[" << i+1<< "][" << j+3 << "] = " << available[i][j] << endl ;
                          lb-- ;
                          no_of_lower_births[i]-- ;
                          no_of_seats_coach[i]-- ;
                          no_of_seats_available-- ;
                        }  

                   }

                   for(j = 1 ; j < 72 && mb > 0 ; j+=8)
                   {  
                      //   cout << "i = " << i+1 << "j = " << j+1 <<"\t" ;
                      if(available[i][j] == 'Y' && mb > 0)
                        {

                          seats[k++] = j+1 ;
                          available[i][j] = 'N' ;
                       //   cout << "in lb1 available[" << i+1<< "][" << j+1 << "] = " << available[i][j] << endl ;
                          mb -- ;
                          no_of_middle_births[i]-- ;
                          no_of_seats_coach[i]-- ;
                          no_of_seats_available-- ;
                        }
                      if(available[i][j+3] == 'Y'&& mb > 0)
                        {
                          seats[k++] = j+4 ;
                          available[i][j+3] = 'N' ;
                        // cout << "in lb2 available[" << i+1<< "][" << j+3 << "] = " << available[i][j] << endl ;
                          mb-- ;
                          no_of_middle_births[i]-- ;
                          no_of_seats_coach[i]-- ;
                          no_of_seats_available-- ;
                        }  

                   }   
                

                
                   for(j = 7 ; j < 72 && su > 0 ; j+=8)
                   {
                      if(available[i][j] == 'Y' && su > 0)
                        {
                          seats[k++] = j+1 ;
                          available[i][j] = 'N' ;
                          //cout << "in su available[" << i+1<< "][" << j+1 << "] = " << available[i][j] << endl ;
                          su -- ;
                          no_of_side_upper[i]-- ;
                          no_of_seats_coach[i]-- ;
                          no_of_seats_available-- ;
                        } 

                   }   

                   for(j = 6 ; j < 72 && sl > 0 ; j+=8)
                   {
                      if(available[i][j] == 'Y' && sl > 0)
                        {
                          seats[k++] = j+1 ;
                          available[i][j] = 'N' ;
                          //cout << "in sl available[" << i+1<< "][" << j+1 << "] = " << available[i][j] << endl ;
                          sl -- ;
                          no_of_side_lower[i]-- ;
                          no_of_seats_coach[i]-- ;
                          no_of_seats_available-- ;
                        }
                   }   
            
             
              char ticket[100] ;
              sprintf(ticket,"ID : %d  CNF Coach : S-%d  Seats : " , tuple.id , i+1) ;
              strcat(s,ticket) ;
              for(j = 0 ; j < k-1 ; j++)
              {
                 sprintf(ticket," %d" , seats[j]) ;
                 strcat(s,ticket) ;
                 if((seats[j]-1)%8 == 2 || (seats[j]-1)%8 == 5)
                       sprintf(ticket,"(UB),") ;

                     if((seats[j]-1)%8 == 0 || (seats[j]-1)%8 == 3)
                       sprintf(ticket,"(LB),") ;

                     if((seats[j]-1)%8 == 7)
                        sprintf(ticket,"(SU),") ;

                     if((seats[j]-1)%8 == 6)
                        sprintf(ticket,"(SL),") ; 
                     
                    if((seats[j]-1)%8 == 1 || (seats[j]-1)%8 == 4)
                       sprintf(ticket,"(MB),") ;

                 strcat(s,ticket) ;
              }
                 sprintf(ticket," %d" , seats[j]) ;
                 strcat(s,ticket) ;
                 if((seats[j]-1)%8 == 2 || (seats[j]-1)%8 == 5)
                       sprintf(ticket,"(UB)") ;

                     if((seats[j]-1)%8 == 0 || (seats[j]-1)%8 == 3)
                       sprintf(ticket,"(LB)") ;

                     if((seats[j]-1)%8 == 7)
                        sprintf(ticket,"(SU)") ;

                     if((seats[j]-1)%8 == 6)
                        sprintf(ticket,"(SL)") ; 
                     
                    if((seats[j]-1)%8 == 1 || (seats[j]-1)%8 == 4)
                       sprintf(ticket,"(MB)") ;

                 strcat(s,ticket) ;
                 return 1 ;
            }
        } 

    


        for( int i = 0 ; i < 12 ; i++)
        {
           if(no_of_seats_coach[i] >= tuple.no_of_booking)
           {
            //cout  << "  YO REAL  "  ;
              int seats[100] ; 
              int k = 0 ;
              int t = tuple.no_of_booking ;
              for(j = 0 ; j < 72 && t > 0 ; j++)
                {
                  if(available[i][j] == 'Y')
                  { 
                      seats[k++] = j+1 ; 
                      available[i][j] = 'N' ;
                     
                      if(j%8 == 2 || j%8 == 5)
                        no_of_upper_births[i]-- ;

                     if(j%8 == 0 || j%8 == 3)
                        no_of_lower_births[i]-- ;

                     if(j%8 == 7)
                        no_of_side_upper[i]-- ;

                     if(j%8 == 6)
                        no_of_side_lower[i]-- ; 
                     
                    if(j%8 == 1 || j%8 == 4)
                       no_of_middle_births[i]-- ;     

                    no_of_seats_coach[i]-- ;
                    no_of_seats_available-- ; 
                    t-- ;                
                  }
                }

              char ticket[100] ;
              sprintf(ticket,"ID : %d  CNF Coach : S-%d  Seats : " , tuple.id , i +1 ) ;
              strcat(s,ticket) ;
              for(j = 0 ; j < k-1 ; j++)
              {
                 sprintf(ticket," %d" , seats[j]) ;
                 strcat(s,ticket) ;
                 if((seats[j]-1)%8 == 2 || (seats[j]-1)%8 == 5)
                       sprintf(ticket,"(UB),") ;

                     if((seats[j]-1)%8 == 0 || (seats[j]-1)%8 == 3)
                       sprintf(ticket,"(LB),") ;

                     if((seats[j]-1)%8 == 7)
                        sprintf(ticket,"(SU),") ;

                     if((seats[j]-1)%8 == 6)
                        sprintf(ticket,"(SL),") ; 
                     
                    if((seats[j]-1)%8 == 1 || (seats[j]-1)%8 == 4)
                       sprintf(ticket,"(MB),") ;

                 strcat(s,ticket) ;
              }
                 sprintf(ticket," %d" , seats[j]) ;
                 strcat(s,ticket) ;
                if((seats[j]-1)%8 == 2 || (seats[j]-1)%8 == 5)
                       sprintf(ticket,"(UB)") ;

                     if((seats[j]-1)%8 == 0 || (seats[j]-1)%8 == 3)
                       sprintf(ticket,"(LB)") ;

                     if((seats[j]-1)%8 == 7)
                        sprintf(ticket,"(SU)") ;

                     if((seats[j]-1)%8 == 6)
                        sprintf(ticket,"(SL)") ; 
                     
                    if((seats[j]-1)%8 == 1 || (seats[j]-1)%8 == 4)
                       sprintf(ticket,"(MB)") ;

                 strcat(s,ticket) ;
                 return 1 ;


           }
        }
      char tickets[100]; 
        sprintf(tickets,"ID : %d  CNF " , tuple.id ) ;
        strcat(s,tickets) ;

        int t = tuple.no_of_booking ;

        for( int i = 0 ; i < 12 && t > 0; i++)
        {


          if(no_of_seats_coach[i] > 0 && t > 0)
          { 
              //cout  << "  YO  " << endl ;
              int seats[100] , k = 0 ;
             for(j = 0 ; j < 72 && no_of_seats_coach[i] > 0 && t > 0; j++)
             {
               if(available[i][j] == 'Y')
                  { 
                    seats[k++] = j+1 ; 
                    available[i][j] = 'N' ;
                   
                    if(j%8 == 2 || j%8 == 5)
                        no_of_upper_births[i]-- ;

                     if(j%8 == 0 || j%8 == 3)
                        no_of_lower_births[i]-- ;

                     if(j%8 == 7)
                        no_of_side_upper[i]-- ;

                     if(j%8 == 6)
                        no_of_side_lower[i]-- ; 
                     
                    if(j%8 == 1 || j%8 == 4)
                       no_of_middle_births[i]-- ; 

                    no_of_seats_coach[i]-- ;
                    no_of_seats_available-- ; 
                    t-- ;    

                  }
               
             }

              char ticket[100] ;
              sprintf( ticket , " Coach : S-%d  Seats : " , i+1 ) ;
              strcat(s,ticket) ;
              for(j = 0 ; j < k-1 ; j++)
              {
                 sprintf(ticket," %d" , seats[j]) ;
                 strcat(s,ticket) ;
                 if((seats[j]-1)%8 == 2 || (seats[j]-1)%8 == 5)
                       sprintf(ticket,"(UB),") ;

                     if((seats[j]-1)%8 == 0 || (seats[j]-1)%8 == 3)
                       sprintf(ticket,"(LB),") ;

                     if((seats[j]-1)%8 == 7)
                        sprintf(ticket,"(SU),") ;

                     if((seats[j]-1)%8 == 6)
                        sprintf(ticket,"(SL),") ; 
                     
                    if((seats[j]-1)%8 == 1 || (seats[j]-1)%8 == 4)
                       sprintf(ticket,"(MB),") ;

                 strcat(s,ticket) ;
              }

              sprintf(ticket," %d" , seats[j]) ;
                 strcat(s,ticket) ;
                 if((seats[j]-1)%8 == 2 || (seats[j]-1)%8 == 5)
                       sprintf(ticket,"(UB)") ;

                     if((seats[j]-1)%8 == 0 || (seats[j]-1)%8 == 3)
                       sprintf(ticket,"(LB)") ;

                     if((seats[j]-1)%8 == 7)
                        sprintf(ticket,"(SU)") ;

                     if((seats[j]-1)%8 == 6)
                        sprintf(ticket,"(SL)") ; 
                     
                    if((seats[j]-1)%8 == 1 || (seats[j]-1)%8 == 4)
                       sprintf(ticket,"(MB)") ;

                 strcat(s,ticket) ;

          }

        }

        return 1 ;

  }

} ;



void SORT(booking list[] ,  vector <int> list2 , int n) 
{

  for (int c = 0 ; c < ( n - 1 ); c++)
  {
    for (int d = 0 ; d < ( n - c - 1) ; d++)
    {
      if(list[d].no_of_booking < list[d+1].no_of_booking)
     {
        booking  swap = list[d] ;
        list[d]      =  list[d+1] ;
        list[d+1]    = swap ;

        int temp  = list2[d] ;
        list2[d]   = list2[d+1]; 
        list2[d+1] = temp ; 
      }

      else if((list[d].no_of_booking == list[d+1].no_of_booking) &&(list[d].no_of_booking < list[d+1].no_of_senior))
      {
        booking  swap = list[d] ;
        list[d]       = list[d+1] ;
        list[d+1]     = swap ;

        int temp  = list2[d] ;
        list2[d]   = list2[d+1]; 
        list2[d+1] = temp ;
       }
    }
  }
}

int main(int argc , char * argv[])
{

    train_ac_rajdhani rajdhani ;
    train_ac_superfast super_ac ;
    train_sleeper_superfast super_sleeper ;

    avail_raj =(int *) mmap(NULL, sizeof *avail_raj, PROT_READ | PROT_WRITE, 
                    MAP_SHARED | MAP_ANONYMOUS, -1, 0); // this is for synchronisation

    avail_ac =(int *) mmap(NULL, sizeof *avail_ac, PROT_READ | PROT_WRITE, 
                    MAP_SHARED | MAP_ANONYMOUS, -1, 0); // this is for synchronisation
    
    avail_sleeper =(int *) mmap(NULL, sizeof *avail_sleeper, PROT_READ | PROT_WRITE, 
                    MAP_SHARED | MAP_ANONYMOUS, -1, 0); // this is for synchronisation
    
   
     *avail_raj = rajdhani.no_of_seats_available ;
     *avail_ac =  super_ac.no_of_seats_available ;
     *avail_sleeper = super_sleeper.no_of_seats_available ; 
     
     pid_t pid1 ;
     
     pid1 = fork() ;

     if(pid1 == 0)
     {
        while(1) 
        {
            int choice ;
            cout << "Press 1  to get the details of the train\nPress 2 to terminate \n" ;
            cin >> choice ;
            if(choice == 1 )
            {
                cout << "     Train                          #seats                                        #seats                 \n"  ;
                cout << "                              Total[Booked/Available]                         Total[Booked/Available]    \n"  ;
                cout << "                                     (AC)                                          (Sleeper)             \n\n";
                printf (" Superfast Exp                   216[%3d,%3d]                                       864[%3d,%3d]         \n",216-(*avail_ac) ,*avail_ac , 864-(*avail_sleeper),*avail_sleeper ) ;            
                printf (" Rajdhani Exp                    702[%3d,%3d]                                          -                 \n",702-(*avail_raj),*avail_raj) ;
                
                cout << "\n\n" ;
           }

           else if(choice == 2)
           {
            exit(0) ;
           } 

        }
     }



    signal( SIGCHLD,SIG_IGN) ;

	int sfd , rst , isSet = 1 , max_sd , sd , max_clients = 50 , client_socket[50] ;
    int new_socket ;
    struct sockaddr_in srv_addr , cli_addr ;  
    socklen_t addrlen = sizeof(struct sockaddr_in) ;
    fd_set read_fds ;    
     
    char *message = "IRCTC BITCH\r\n";
    for(int i = 0 ; i < max_clients ; i++)
    	client_socket[i] = 0 ;

    sfd = socket(AF_INET , SOCK_STREAM , 0 ) ;
    setsockopt(sfd , SOL_SOCKET , SO_REUSEADDR , (char *)&isSet,sizeof(isSet)) ;

    if(sfd == -1)
    {
    	perror("Socket error : Failed to create a socket") ;
    	exit(0) ;
    }
     

     memset (&srv_addr, 0, addrlen);
     memset (&cli_addr, 0, addrlen);

   
    srv_addr.sin_family = AF_INET; // IPv4.
    srv_addr.sin_port   = htons (port_number);
    srv_addr.sin_addr.s_addr = INADDR_ANY;
    //rst = inet_pton (AF_INET, "127.0.0.1", &srv_addr.sin_addr);
  
    rst  = bind(sfd , (struct sockaddr *) &srv_addr , addrlen) ;
    
     if (rst == -1)
    {
        perror ("Failure in binding");
        exit (0);
    }   

    printf("Listener on port %d \n", port_number);  
    
    //printf ("Max connections allowed to wait: %d\n", SOMAXCONN);
    rst = listen (sfd, 50);
     
     if (rst == -1)
    {
        perror ("Listen error : ");
        exit (0);
    }  

   // cout << "Waiting for connections...." << endl ;
    

    while(true)
    {   
    	//cout << "1" << endl ;
    	FD_ZERO(&read_fds) ;
    	FD_SET(sfd , &read_fds) ;

    	max_sd = sfd ;

    	for(int i = 0 ; i < max_clients ; ++i)
    	{
    		sd = client_socket[i] ;
            if(sd > 0)
                FD_SET( sd , &read_fds);

            if(sd > max_sd)
            	max_sd = sd ;
    	}

        //cout << "2" << endl ;

    	rst = select(max_sd+1 , &read_fds, NULL , NULL , NULL) ;
        
        if((rst < 0) && (errno != EINTR))
        {
        	 perror("Select error") ;
        	 exit(0) ;
        }

        if (FD_ISSET(sfd, &read_fds)) 
        {
            if ((new_socket = accept(sfd, (struct sockaddr *)&srv_addr, &addrlen))<0)
            {
                perror("Accept error");
                exit(EXIT_FAILURE);
            }
          
            //inform user of socket number - used in send and receive commands
            //printf("New connection , socket fd is %d , ip is : %s , port : %d \n" , new_socket , inet_ntoa(srv_addr.sin_addr) , ntohs(srv_addr.sin_port));
        
            //INITIAL TESTING PHASE
            if( send(new_socket, message, strlen(message), 0) != strlen(message) ) 
            {
                perror("send");
            }
              
            //puts("Welcome message sent successfully");
              
            //add new socket to array of sockets
            for (int i = 0; i < max_clients; i++) 
            {
                //if position is empty
                if( client_socket[i] == 0 )
                {
                    client_socket[i] = new_socket;
                    //printf("Adding to list of sockets as %d\n" , i);
                     
                    break;
                }
            }
        }

         
         booking book_list[100]  ; int  d = 0 ;
         vector <int> id_list ;

        for (int i = 0; i < max_clients; i++) 
        {
            sd = client_socket[i];
                 
            if (FD_ISSET( sd , &read_fds)) 
            {
            
            	int valread  ;char buffer[1025];
            	  bzero(buffer,1025);
                //Check if it was for closing , and also read the incoming message
                if ((valread = read( sd , buffer, 1024)) == 0)
                {
                	//cout << "4" << endl ;

                    //Somebody disconnected , get his details and print
                    getpeername(sd , (struct sockaddr*)&srv_addr ,&addrlen);
                    //printf("Host disconnected , ip %s , port %d \n" , inet_ntoa(srv_addr.sin_addr) , ntohs(srv_addr.sin_port));
        
                    //Close the socket and mark as 0 in list for reuse
                    close( sd );
                    client_socket[i] = 0;
                }
                 
                //Echo back the message that came in
               else
                {
                	book_list[d++] = parser(buffer) ;
                    id_list.push_back(i) ; 
                }
            }
        }


        SORT(book_list ,id_list , d) ;

        char ticket_info[500] ;
        for(int i = 0 ; i < id_list.size() ; i++)
        {
            sd = client_socket[id_list[i]];
            if(book_list[i].train_no == 12301)
            {
                ticket_info[0] = '\0' ; 
                strcat(ticket_info , "AC-RAJDHANI(12301) : ");
                rajdhani.book_function(book_list[i] , ticket_info) ;
                send(sd,ticket_info,500,0);
            }

            else 
            {
                if(book_list[i].type == 1 ) 
                {
                    ticket_info[0] = '\0' ; 
                    strcat(ticket_info , "AC-SUPERFAST(12321) : ");
                    super_ac.book_function(book_list[i] , ticket_info) ;
                    send(sd,ticket_info,500,0) ;
                }

                if(book_list[i].type == 2 ) 
                {
                    ticket_info[0] = '\0' ; 
                    strcat(ticket_info , "SLEEPER_SUPERFAST(12321) : ");
                    super_sleeper.book_function(book_list[i] , ticket_info) ;
                    send(sd,ticket_info,500,0) ;
                }
            }

            *avail_raj = rajdhani.no_of_seats_available ;
            *avail_ac =  super_ac.no_of_seats_available ;
            *avail_sleeper = super_sleeper.no_of_seats_available ;
        }

        



    }   
   
    return 0 ;
    /*
    while(1)
    {
    	
    	read_fds = active_fds ;
    	rst = select(FD_SETSIZE  , &read_fds , NULL , NULL , NULL) ;
    	if(rst < 0) 
    	{
           perror("Select error") ;
    	}

    	for(int i = 0 ; i < FD_SETSIZE ; ++i)
    	{
           if(FD_ISSET(i,&read_fds))
           {
                int temp = 
           }


    	}


    }
    */
}
 