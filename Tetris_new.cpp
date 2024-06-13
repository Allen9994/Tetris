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
bool flag = false;
short temp = 0;
char wall[] = {':','|'};
const short side = 10;
const short area = side * side;
short speed = 1000, level = 2, pace = 1, head = 22;
char value = 'q';
short p[area-1] = {0};
vector<int> trail(1, 0); 
string map(area,' ');
short highscore = 0;
short last = 0;
short length = 0;
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

void deletion()
{
    int side = 2;
    int i;
    short num = 0;
    bool flag = false;
    int map[] = {0,1,0,1,0,0,1,1,1,0,0,0,0,1,0,0};
    int area = sizeof(map)/sizeof(int);
    int j = area-1;
    while(j>=side)
    {
        if(map[j] == 1) 
        {
            j = (j/side)*side;
        }
        else if(j%side == 0)
        {
            i = j-1;
            num++;
            while(i>0)
            {
                map[i+side] = map[i];
                i--;
            }
        }
        j--;
    }
    for(i=0;i<num*side;i++)
    {
        map[i] = 0;
    }
    for(i=0;i<area;i++)
    {
        cout<<map[i]<<" ";
    }
}
void demolish()
{
    j = area-1;
    while(j > 0)
    {
        if (j%10 != 0 and map[j] != ' ') flag = true;
        else j = (j/10)*10 - 1;
        if (flag == true)
        {
            map[j] = map[j+side];
        }
        j--;
    }flag = false;
}
void convert()
{
    switch (p[counter])
    {
        case 1:length = 1;break;
        case 2:length = 2;break;
        case 3:length = 3;break;
        case 4:length = 4;break;
        case 5:length = 1;break;
        case 6:length = 2;break;
        case 7:length = 2;break;
    }
}
void randomize()
{
    srand((unsigned) time(0));
    for (short index = 0; index < area; index++) 
    {
        p[index] = (rand() % 6) + 1;
    }
}
void shape()
{
    map[head] = 'x';
    map[last] = ' ';
    if(p[counter] == 2)
    {
        map[head+1] = 'x';
        map[last+1] = ' ';
        if (map[head+side+1] == 'x') head = 2;
    }
    if(p[counter] == 3)
    {
        map[head+1] = 'x';
        map[last+1] = ' ';
        map[head-side+1] = 'x';
        map[last-side+1] = ' ';
        map[head-side+2] = 'x';
        map[last-side+2] = ' ';
        if (map[head+side+1] == 'x' || map[head+2] == 'x') head = 2;
    }
    if(p[counter] == 4)
    {
        map[head+1] = 'x';
        map[last+1] = ' ';
        map[head+2] = 'x';
        map[last+2] = ' ';
        map[head+3] = 'x';
        map[last+3] = ' ';
        if (map[head+side+1] == 'x' || map[head+side+2] == 'x' || map[head+side+3] == 'x') head = 2;
    }
    if(p[counter] == 5)
    {
        map[head-side] = 'x';
        map[last-side] = ' ';
    }
    if(p[counter] == 6)
    {
        map[head-side] = 'x';
        map[last-side] = ' ';
        map[head+1] = 'x';
        map[head-side+1] = 'x';
        map[last-side+1] = ' ';
        if (map[head+side+1] == 'x') head = 2;
    }
    if (map[head+side] == 'x') head = 2;
    if (head >= area-side && head <= area) head = 2;
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
    if (value == 'd' && map[head+length+1] != 'x' && head%side != side-length-1) head++;
    if (value == 'a' && map[head-1] != 'x' && head%side != 0) head--;
    process();
}
void process()   //Brain of the program. Entire game operation happens here. 
{
    system("clear");
    if (map[head+side] == 'x' && head <= side) gameToggle(side,false);
    head+=side;
    cout<<head<<endl;
    value = 'q';
    shape();
    if (head == 2) {
        demolish();
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
