//Start of the program
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <stdlib.h>
#include <termios.h> //termios, TCSANOW, ECHO, ICANON
#include <unistd.h> 
#include <time.h>
using namespace std;

short score = 0;
short points = 0;
short i,j;
char wall[] = {':','|'};
const short side = 10;
const short area = side * side;
short speed = 1000, level = 2, pace = 1, head = 5;
char value = 'q';
short p[area-1] = {0};
vector<int> trail(1, 0); 
string map(area,' ');
short highscore = 0;
short last = 0;
string uline(side+1,'_');
string bline(side+1,'"');

condition_variable cv;
void control(char);
void display();
void gameToggle(short,bool);
void process();
void mainMenu();
void fileManage(string,char);
void speedSelector();

void randomize()
{
    srand((unsigned) time(0));
    for (short index = 0; index < area/2; index++) 
    {
        p[index] = (rand() % area-2) + 1;
    }
}

void read_value() //inputting value from user
{   
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
 
    char c = getchar();
    switch(c)
    {
        case 'd': 
        case 'a': value = c;
    }

 /* restore the old settings */
tcsetattr( STDIN_FILENO, TCSANOW, &oldt);
  
    cv.notify_one();
}
void take_input() //function to accept the value parallelly while game is proceeding.  
{
    thread th(read_value);
    mutex mtx;
    unique_lock<mutex> lck(mtx);
    while (cv.wait_for(lck, chrono::milliseconds(speed)) == cv_status::timeout) control(value);
    th.join();
    control(value);
}
void control(char value) //Converts user input to the direction snake must move and stores all the movements into the array
{
    last = head;
    if (value == 'd' && map[head+1] != 'x' && head%10 != 8) head++;
    if (value == 'a' && map[head-1] != 'x' && head%10 != 0) head--;
    process();
}
void process()   //Brain of the program. Entire game operation happens here. 
{
    system("clear");
    cout<<head<<endl;
    if (map[head+side] == 'x' && head <= side) gameToggle(side,false);
    head+=side;
    value = 'q';
    map[head] = 'x';
    map[last] = ' ';
    if (map[head+side] == 'x') head = 2;
    if (head>= area-side && head <= area) head = 2;
    display();
}
void display()
{
    cout<<uline<<endl;
    for (j=0;j<side;j++)  //Designing the 2Dmodel : Borders not made yet
    {   
        for (i=0;i<side;i++)
        {
            if(i == side-1 || i == 0)       cout<<"|";
            if(i == side-1 && j == side-1)  cout<<endl<<bline;
            cout<<map[(j*side)+i];
        }
        cout<<endl;
    }
}
int main()  
{ 
    fileManage("0",'i');
    randomize();
    mainMenu();
}
void gameToggle(short score, bool toggle) //To exit the game when the snake bites itself
{   
    do{
        if(!toggle)
        {
            system("clear");
            fileManage(to_string(pace) + to_string(level),'s');
            fileManage(to_string(score),'o');
            cout<<"Game Over!\nScore:"<<score;
            exit(0);
        }
        take_input();
    }while(1);
}
void mainMenu()   //Main Menu
{   
    gameToggle(0,true);
    char choice='z';
    cout<<"\nCreated by Allen\n";
    cout<<"Press:\n1 to Play\n2 for Help\n3 for Game Settings\n4 to exit\n";
    cin>>choice;
    if(choice == '1') gameToggle(0,true);
    if(choice == '2')   //Instructions
    {   
        system("clear");
        cout<<"CONTROLS\nPRESS\n w TO MOVE UPWARD\n s TO MOVE DOWNWARDS \n d TO MOVE RIGHT \n a TO MOVE LEFT";
        cin>>choice;
        system("clear");
        main();
    }
    if(choice == '3')
    {
        system("clear");
        cout<<"Control the Snake Speed. PRESS\n1 : Easy\n2 : Medium\n3 : Hard\n";
        cin>>pace;
        cout<<"Control the Game Difficulty level. PRESS\n1 : LEVEL 1\n2 : LEVEL 2\n";
        cin>>level;
        level = (level == 1) ? 1 : 2;
        speedSelector();
        gameToggle(0,true);
    }
    else exit(0);
}
void fileManage(string data, char option)
{
    if(option == 'i')
    {
        ifstream fin("snakes_data.txt"); 
        if(!fin) cout<<"Welcome to the game!"; 
        else
        {
            string s;
            cout<<"Welcome back to the game!\nThe highscore is ";
            while (fin.good()) {
                getline(fin,s);
            }
            pace =  s[0] - '0';
            level = s[1] - '0';
            if (highscore < stoi(s.substr(2))) highscore = stoi(s.substr(2)) ;
            cout<<highscore;
            speedSelector();
        }
        fin.close();
    }
    else if(option == 's') {
        ofstream fout("snakes_data.txt",ios::app);
        fout<<endl<<data;
        fout.close();
    }
    else if(option == 'o')
    {
        ofstream fout("snakes_data.txt", ios::app);
        if(stoi(data) > highscore) 
        {
            highscore = stoi(data);
            cout<<"HIGHSCORE! "<<highscore<<endl;
        }
        fout<<highscore;
        fout.close();
    }
}
void speedSelector()
{
    switch(pace)
    {
        case 1: speed = 800;break;
        case 3: speed = 800;break;
        default:speed = 800;pace = 2;break;
    }
}
