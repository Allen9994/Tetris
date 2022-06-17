#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif
#include <iostream>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>
#include<stdlib.h>
#include<stdio.h>
 #include <termios.h> //termios, TCSANOW, ECHO, ICANON
 #include <unistd.h> 
#include <time.h>

static int score=0;
using namespace std;


condition_variable cv;
void kill(int);
void lame(int,int);
int value = 2;
int choice = 1;
int step = 0;
int toy[96]={0};
void calc(int);
void helpscreen();



void decide()
{
    srand (time(NULL));
    choice = rand() % 4 + 1;   
    cout<<choice;
}



void read_value() //inputting value from user
{   

  int c=0;
 static struct termios oldt, newt;

 /* tcgetattr gets the parametervas of the current terminal
STDIN_FILENO will tell tcgetattr that it should write the
settings of stdin to oldt */
 tcgetattr( STDIN_FILENO, &oldt);
 /*now the settings will be copied*/
 newt = oldt;

 /* ICANON normally takes care that one line at a time will be
processed that means it will return if it sees a "\n" or an EOF
or an EOL */
 newt.c_lflag &= ~(ICANON);

 /* Those new settings will be set to STDIN TCSANOW tells
tcsetattr to change attributes immediately. */
 tcsetattr( STDIN_FILENO, TCSANOW, &newt);
 
    c = getchar();
    value =  (int)c - 48;

 /* restore the old settings */
 tcsetattr( STDIN_FILENO, TCSANOW, &oldt);
  
    cv.notify_one();
}
void takin() //function to accept the value parallelly while game is proceeding.  
{
    value = 2;
    thread th(read_value);
    mutex mtx;
    unique_lock<mutex> lck(mtx);
    while (cv.wait_for(lck, chrono::milliseconds(400)) == cv_status::timeout)
    {
        kill(value);
    }

    th.join();

    kill(value);
}
void kill(int value) //Converts user input to the direction snake must move and stores all the movements into the array
{   
    static int i=2;
    if(step == 1)
    {
        i=2;
        step = 0;
    }
    switch (choice)
    {
        case 1:
        if(toy[i+6]!=1)
        {
            i+=6;
            calc(i);
        }break;
        case 2:
        if(toy[i+6]!=1&&toy[i+7]!=1&&toy[i]!=1&&toy[i-1]!=1)
        {
            i+=6;
            calc(i);
        }break;
        case 3:
        if(toy[i+6]!=1&&toy[i+7]!=1)
        {
            i+=6;
            calc(i);
        }break;
        case 4:
        if(toy[i+6]!=1&&toy[i+7]!=1&&toy[i+5]!=1&&toy[i+8]!=1)
        {
            i+=6;
            calc(i);
        }break;
    
    }
    
    if(value==3)
    {   
        i++;
        calc(i);
    }
    else
    if(value==1)
    {
        i--;
        calc(i);
    }
}
void calc(int n)   //Brain of the program. Entire game operation happens here. 
{
    int l;
    system("clear");
    int i ;
    
    switch (choice)
    {
        case 1:
        if(toy[n+6]==2||toy[n+6]==1)
        {
            toy[n] = 1;
            step = 1;
            decide();
        }break;
        case 2:
        if(toy[n+7]==2||toy[n+7]==1||toy[n+6]==2||toy[n+6]==1||toy[n]==2||toy[n]==1||toy[n-1]==2||toy[n-1]==1)
        {
            toy[n] = 1;toy[n+1] = 1;toy[n-6] = 1;toy[n-7] = 1;
            step = 1;
            decide();
        }break;
        case 3:
        if(toy[n+7]==2||toy[n+7]==1||toy[n+6]==2||toy[n+6]==1)
        {
        
            toy[n] = 1;toy[n+1] = 1;
            step = 1;
            decide();
        }break;
        case 4:
        if(toy[n+7]==2||toy[n+7]==1||toy[n+6]==2||toy[n+6]==1||toy[n+5]==2||toy[n+5]==1||toy[n+8]==2||toy[n+8]==1)
        {
            toy[n] = 1;toy[n+1] = 1;toy[n-1] = 1;toy[n+2] = 1;
            step = 1;
            decide();
        }
        
    }
    
    int a[90]={0};
    for(i=0;i<90;i++)
    {
        a[i]= toy[i];
    }
    
    int j,z=0;
    
     //Storing the values to another array
    static int x[15][6] = {0};
      //For identifying the previous position and the new position of the snake
    switch (choice){
    case 1:
        a[n] = 1;break;
    case 2:
        a[n] = 1;
            a[n+1] = 1;
            a[n-6] = 1;
            a[n-7] = 1;
        break;
    case 3:
            a[n] = 1;
            a[n+1] = 1;
        break;
    case 4:
        a[n] = 1;
            a[n+1] = 1;
            a[n-1] = 1;
            a[n+2] = 1;
        break;
    }
    int pp;        
    for(i=0;i<15;i++)
    {
        pp=i*6;
        if(toy[pp]==1&&toy[pp+1]==1&&toy[pp+2]==1&&toy[pp+3]==1&&toy[pp+4]==1&&toy[pp+5]==1)
        {
            for(j=pp+5;j>0;j--)
            {
                toy[j]=toy[j-6];
            }
        }
    }
    
    
    for (i=0;i<15;i++)  //converting 1D array of snake's movements into 2D array 
    {   
        for (j=0;j<6;j++)
        {
            z=i*6+j;
            x[i][j]=a[z];
        }
        
    }
    
    
    cout<<endl;
    cout<<"_______"<<endl;
    for (i=0;i<15;i++)  //Designing the 2Dmodel : Borders not made yet
    {   
        for (j=0;j<6;j++)
        {
            if(j==0)
            {
                cout<<"|";
            }
           if(x[i][j]==0)
            {
                cout<<" ";
            } else 
            if(x[i][j]==1){
                cout<<"X";
            }
                
            }cout<<endl;
    }
    
}

int main()  
{   
    int i;
    for(i=0;i<6;i++)
    {
        toy[90+i]=2;
    }
    lame(0,0);
}
void lame(int sc,int k) //To exit the game when the snake bites itself
{  
    do{
        if(k==420)
        {
            cout<<"LOST";
            cout<<"\nScore:"<<sc;
            exit(0);
        }
        takin();
    }while(1);
}
