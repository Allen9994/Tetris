//Start of the program
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif
#include <iostream>
#include <fstream>
#include <algorithm>
#include <chrono>
#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <stdlib.h>
#include <termios.h> //termios, TCSANOW, ECHO, ICANON
#include <unistd.h> 
#include <time.h>
using namespace std;

short score = 0;
short points = 0;
short i,j,pos;
short m = 0;
bool plag = false;
short block = 0;
const short side = 14;
short num,k;
const short area = side * side;
short speed = 1000, level = 2, pace = 1, head = side/4;
char value = 'q';
short p[area-1] = {0};
string map(area,' ');
short highscore = 0;
short last = 0;
vector <short>u;
vector <short>v;
short block_length = 0,block_height = 0;
short counter = 2;

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

void destroy()
{
    pos = area-1;
    while(pos >= side)
    {
        if(map[pos] == 'x') block++;
        if(block == side-1)
        {
            score++;
            k = pos-1;
            while(k >= 0)
            {
                map[k+side] = map[k];
                k--;
            }
            pos += side;
        }
        if (pos%side == 0) block = 0;
        pos--;
    }
    block = 0;
}
void convert()
{
    v.clear();u.clear();
    switch (p[counter])
    {
        case 1:v = {0};         u = {1};            block_length=1;block_height=0;break;
        case 2:v = {1};         u = {1};            block_length=2;block_height=1;break;
        case 3:v = {1,-side+2}; u = {1,-side};      block_length=3;block_height=1;break;
        case 4:v = {3};         u = {1};            block_length=4;block_height=0;break;
        case 5:v = {0,side};    u = {1,-side-1};    block_length=1;block_height=2;break;
        case 6:v = {1,-side+1}; u = {1,-side-1};    block_length=2;block_height=1;break;
        case 7:v = {1};         u = {1,-side-1};    block_length=2;block_height=1;break;
        case 8:v = {0,-side};   u = {0,-side-1};    block_length=1;block_height=1;break;
        case 9:v = {-side+1};   u = {1};            block_length=2;block_height=1;break;
        case 10:v = {-side};    u = {1,-side-2};    block_length=1;block_height=1;break;
    }
}
void randomize()
{
    srand((unsigned) time(0));
    for (short index = 0; index < area; index++) p[index] = (rand() % 10) + 1;
}
void shape()
{
    map[last] = ' ';
    map[head] = 'x';
    if(p[counter] == 2)
    {
        map[last+1] = ' ';
        map[head+1] = 'x';
        if (map[head+side+1] == 'x') head = side/4;
    }
    if(p[counter] == 3)
    {
        map[last+1] = map[last-side+1] = map[last-side+2] = ' ';
        map[head+1] = map[head-side+1] = map[head-side+2] = 'x';
        if (map[head+side+1] == 'x' || map[head+2] == 'x') head = side/4;
    }
    if(p[counter] == 4)
    {
        map[last+1] = map[last+2] = map[last+3] = ' ';
        map[head+1] = map[head+2] = map[head+3] = 'x';
        if (map[head+side+1] == 'x' || map[head+side+2] == 'x' || map[head+side+3] == 'x') head = side/4;
    }
    if(p[counter] == 5)
    {
        map[last-side] = ' ';
        map[head-side] = 'x';
    }
    if(p[counter] == 6)
    {
        map[last-side] = map[last+1] = map[last-side+1] = ' ';
        map[head-side] = map[head+1] = map[head-side+1] = 'x';
        if (map[head+side+1] == 'x') head = side/4;
    }
    if(p[counter] == 7)
    {
        map[last-side] = map[last+1] = ' ';
        map[head-side] = map[head+1] = 'x';
        if (map[head+side+1] == 'x') head = side/4;
    }
    if(p[counter] == 8)
    {
        map[last-side] = map[last-1] = ' ';
        map[head-side] = map[head-1] = 'x';
        if (map[head+side-1] == 'x') head = side/4;
    }
    if(p[counter] == 9)
    {
        map[last-side] = map[last-side+1] = ' ';
        map[head-side] = map[head-side+1] = 'x';
        if (map[head+1] == 'x') head = side/4;
    }
    if(p[counter] == 10)
    {
        map[last-side] = map[last-side-1] = ' ';
        map[head-side] = map[head-side-1] = 'x';
        if (map[head-1] == 'x') head = side/4;
    }
    if (map[head+side] == 'x' || (head >= area-side && head <= area)) head = side/4;
}
void read_value() //inputting value from user
{   
    static struct termios oldt, newt;
    tcgetattr( STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON);
    tcsetattr( STDIN_FILENO, TCSANOW, &newt);
 
    char c = getchar();
    switch(c)
    {
        case 'd': 
        case 'a': value = c;
    }
    if (p[counter]== 6 && (value == 'a' || value == 'd') && (map[head+side] != 'x' || map[head+side+1] != 'x')) map[last+1] = ' ';
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
    convert();
    if (value == 'd')
    {
        m = 0;plag = false;
        while(m < v.size())
        {
            if(map[head+side+v[m]+2] == 'x' || head%side == side-block_length-1) plag = true;
            m++;
        }
        if(!plag) head++;
        plag = false;
    }
    if (value == 'a')
    {
        m = 0;plag = false;
        while(m < u.size())
        {
            if(map[head+side+u[m]-2] == 'x' || head%side == 0) plag = true;
            m++;
        }
        cout<<plag;
        if(!plag) head--;
        plag = false;
    }
    if (value == 's') head += side;
    process();
}
void process()   //Brain of the program. Entire game operation happens here. 
{
    system("clear");
    if (map[head+side] == 'x' && head-(block_height)*side <= side) gameToggle(score,false);
    head += side;
    value = 'q';
    shape();
    if (head == side/4) {
        destroy();
        counter++;
        convert();
    }
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
void gameToggle(short score, bool toggle) 
{   
    do{
        if(toggle) take_input();
        else
        {
            fileManage(to_string(pace) + to_string(level),'s');
            fileManage(to_string(score),'o');
            cout<<"Game Over!\nScore:"<<score;
            exit(0);
        }
    }while(toggle);
}
void mainMenu()   //Main Menu
{   
    gameToggle(0,true);
    char choice = 'z';
    cout<<"\nCreated by Allen\n";
    cout<<"Press:\n1 to Play\n2 for Help\n3 for Game Settings\n4 to exit\n";
    cin>>choice;
    system("clear");
    if(choice == '1') gameToggle(0,true);
    if(choice == '2')   //Instructions
    {   
        cout<<"CONTROLS\nPRESS\n w TO MOVE UPWARD\n s TO MOVE DOWNWARDS \n d TO MOVE RIGHT \n a TO MOVE LEFT";
        cin>>choice;
        system("clear");
        main();
    }
    if(choice == '3')
    {
        cout<<"Control the Block Speed. PRESS\n1 : Easy\n2 : Medium\n3 : Hard\n";
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
        ifstream fin("tetris_data.txt"); 
        if(!fin) cout<<"Welcome to the game!"; 
        else
        {
            string s;
            cout<<"Welcome back to the game!\nThe highscore is ";
            while (fin.good()) {
                getline(fin,s);
            }
            pace  = s[0] - '0';
            level = s[1] - '0';
            if (highscore < stoi(s.substr(2))) highscore = stoi(s.substr(2)) ;
            cout<<highscore;
            speedSelector();
        }
        fin.close();
    }
    else if(option == 's') {
        ofstream fout("tetris_data.txt",ios::app);
        fout<<endl<<data;
        fout.close();
    }
    else if(option == 'o')
    {
        ofstream fout("tetris_data.txt", ios::app);
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
