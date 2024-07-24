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

const short side = 14;
short i,j,levelCounter,score = 0, figure = 0, levelShift, block = 0;
short last = 0,highscore = 0, previewFigure = 0, listCounter = 2;
short speed = 1000, pace = 1, head = (side-1)/2;
short block_length = 0, block_height = 0, block_width = 0;
bool hitWall = false;
const short area = side * side;
char value = 'm';
short shapeList[area-1] = {0};
string map(area,' ');
vector <short>u;
vector <short>v;
string preview = "";
string uline(side+1,'_');
string bline(side+1,'"');

condition_variable cv;
void control();
void display();
void gameToggle(bool);
void process();
void mainMenu();
void fileManage(string,char);
void speedSelector();

void blockPreview()
{
    previewFigure = shapeList[listCounter];
    switch(previewFigure)
    {
        case 1: preview = "x\n\n\n";        break;
        case 2: preview = "xx\n\n\n";       break;
        case 3: preview = " xx\nxx\n\n";    break;
        case 4: preview = "xxxx\n\n\n";     break;
        case 5: preview = "x\nx\n\n";       break;
        case 6: preview = "xx\nxx\n\n";     break;
        case 7: preview = "x\nxx\n\n";      break;
        case 8: preview = " x\nxx\n\n";     break;
        case 9: preview = "xx\nx\n\n";      break;
        case 10:preview = "xx\n x\n\n";     break;
        case 11:preview = " x\nxx\nx\n";    break;
        case 12:preview = "x\nxx\n x\n";    break;
        case 13:preview = "xx\n xx\n\n";    break;
        case 14:preview = "x\nx\nx\nx";     break;
        case 15:preview = " x \nxxx\n\n";   break; 
        case 16:preview = "xxx\n x \n\n";   break;
        case 17:preview = "x\nxx\nx\n";     break;
        case 18:preview = " x\nxx\n x\n";   break;
        case 19:preview = " x \nxxx\n x \n";break;
    }
    cout<<preview<<endl;
}
void randomize()
{
    srand((unsigned) time(0));
    for (short index = 0; index < area; index++) shapeList[index] = (rand() % 19) + 1;
    figure = shapeList[listCounter];
}
void destroyBlocks()
{
    figure = shapeList[listCounter];
    levelCounter = area-1;
    while(levelCounter >= side)
    {
        if(map[levelCounter] == 'x') block++;
        if(block == side-1){
            score++;
            levelShift = levelCounter-1;
            while(levelShift >= 0){
                map[levelShift+side] = map[levelShift];
                levelShift--;
            }
            levelCounter += side;
        }
        if (levelCounter%side == 0) block = 0;
        levelCounter--;
    }
    block = 0;
}
void convert()
{
    v.clear();
    u.clear();
    switch (figure)
    {
        case 1:v = {0};                     u = {1};                            block_length=1;block_height=0;block_width=0;break;
        case 2:v = {1};                     u = {1};                            block_length=2;block_height=0;block_width=0;break;
        case 3:v = {1,-side+2};             u = {1,-side};                      block_length=3;block_height=1;block_width=0;break;
        case 4:v = {3};                     u = {1};                            block_length=4;block_height=0;block_width=0;break;
        case 5:v = {0,-side};               u = {1,-side-1};                    block_length=1;block_height=1;block_width=0;break;
        case 6:v = {1,-side+1};             u = {1,-side-1};                    block_length=2;block_height=1;block_width=0;break;
        case 7:v = {1,-side+1};             u = {1,-side-1};                    block_length=2;block_height=1;block_width=0;break;
        case 8:v = {0,-side};               u = {0,-side};                      block_length=1;block_height=1;block_width=1;break;
        case 9:v = {0,-side+1};             u = {1,-side-1};                    block_length=2;block_height=1;block_width=0;break;
        case 10:v= {0,-side};               u = {1,-side-2};                    block_length=1;block_height=1;block_width=1;break;
        case 11:v= {0,-side+1,(-2*side)+1}; u = {1,-side-1,-2*side};            block_length=2;block_height=2;block_width=0;break;
        case 12:v= {0,-side,(-2*side)};     u = {1,-side-1,-2*side-1};          block_length=1;block_height=2;block_width=1;break;
        case 13:v= {0};                     u = {0,-side-3};                    block_length=1;block_height=1;block_width=2;break;  
        case 14:v= {0,-side,2*-side,3*-side};u = {1,-side-1,-2*side-1,-3*side-1};block_length=1;block_height=3;block_width=0;break;
        case 15:v= {1,-side+1};             u = {0,-side};                      block_length=2;block_height=1;block_width=1;break;
        case 16:v= {0,-side+1};             u = {1,-side-2};                    block_length=2;block_height=1;block_width=1;break;
        case 17:v= {0,-side+1,(2*side)};    u = {1,-side-1,(2*side)-1};         block_length=2;block_height=2;block_width=0;break;
        case 18:v= {0,-side,(2*side)};      u = {1,-side-2,(2*side)-1};         block_length=1;block_height=2;block_width=1;break;
        case 19:v= {0,-side+1,(2*side)};    u = {1,-side-2,(2*side)-1};         block_length=2;block_height=2;block_width=1;break;
    }
}
void createShape()
{
    map[last] = ' ';
    map[head] = 'x';
    if(figure == 2){
        map[last+1] = ' ';
        map[head+1] = 'x';
        if (map[head+side+1] == 'x') head = (side-1)/2;
    }
    if(figure == 3){
        map[last+1] = map[last-side+1] = map[last-side+2] = ' ';
        map[head+1] = map[head-side+1] = map[head-side+2] = 'x';
        if (map[head+side+1] == 'x' || map[head+2] == 'x') head = (side-1)/2;
    }
    if(figure == 4){
        map[last+1] = map[last+2] = map[last+3] = ' ';
        map[head+1] = map[head+2] = map[head+3] = 'x';
        if (map[head+side+1] == 'x' || map[head+side+2] == 'x' || map[head+side+3] == 'x') head = (side-1)/2;
    }
    if(figure == 5){
        map[last-side] = ' ';
        map[head-side] = 'x';
    }
    if(figure == 6){
        map[last-side] = map[last+1] = map[last-side+1] = ' ';
        map[head-side] = map[head+1] = map[head-side+1] = 'x';
        if (map[head+side+1] == 'x') head = (side-1)/2;
    }
    if(figure == 7){
        map[last-side] = map[last+1] = ' ';
        map[head-side] = map[head+1] = 'x';
        if (map[head+side+1] == 'x') head = (side-1)/2;
    }
    if(figure == 8){
        map[last-side] = map[last-1] = ' ';
        map[head-side] = map[head-1] = 'x';
        if (map[head+side-1] == 'x') head = (side-1)/2;
    }
    if(figure == 9){
        map[last-side] = map[last-side+1] = ' ';
        map[head-side] = map[head-side+1] = 'x';
        if (map[head+1] == 'x') head = (side-1)/2;
    }
    if(figure == 10){
        map[last-side] = map[last-side-1] = ' ';
        map[head-side] = map[head-side-1] = 'x';
        if (map[head-1] == 'x') head = (side-1)/2;
    }
    if(figure == 11){
        map[last-side] = map[last-side+1] = map[last-(2*side)+1] = ' ';
        map[head-side] = map[head-side+1] = map[head-(2*side)+1] = 'x';
        if (map[head+1] == 'x') head = (side-1)/2;
    }
    if(figure == 12){
        map[last-side] = map[last-side-1] = map[last-(2*side)-1] = ' ';
        map[head-side] = map[head-side-1] = map[head-(2*side)-1] = 'x';
        if (map[head-1] == 'x') head = (side-1)/2;
    }
    if(figure == 13){
        map[last-1] = map[last-side-1] = map[last-side-2] = ' ';
        map[head-1] = map[head-side-1] = map[head-side-2] = 'x';
        if (map[head+side-1] == 'x' || map[head-2] == 'x') head = (side-1)/2;
    }
    if(figure == 14){
        map[last-side] = map[last-(2*side)]= map[last-(3*side)] = ' ';
        map[head-side] = map[head-(2*side)]= map[head-(3*side)] = 'x';
    }
    if(figure == 15){
        map[last-1] = map[last+1] = map[last-side] = ' ';
        map[head-1] = map[head+1] = map[head-side] = 'x';
        if (map[head+side+1] == 'x'|| map[head+side-1] == 'x') head = (side-1)/2;
    }
    if(figure == 16){
        map[last-side] = map[last-side+1] = map[last-side-1] = ' ';
        map[head-side] = map[head-side+1] = map[head-side-1] = 'x';
        if (map[head+1] == 'x'|| map[head-1] == 'x') head = (side-1)/2;
    }
    if(figure == 17){
        map[last-side] = map[last-(2*side)] = map[last-side+1] = ' ';
        map[head-side] = map[head-(2*side)] = map[head-side+1] = 'x';
        if (map[head+1] == 'x') head = (side-1)/2;
    }
    if(figure == 18){
        map[last-side] = map[last-(2*side)] = map[last-side-1] = ' ';
        map[head-side] = map[head-(2*side)] = map[head-side-1] = 'x';
        if (map[head-1] == 'x') head = (side-1)/2;
    }
    if(figure == 19){
        map[last-side] = map[last-(2*side)] = map[last-side-1] = map[last-side+1] = ' ';
        map[head-side] = map[head-(2*side)] = map[head-side-1] = map[head-side+1] = 'x';
        if (map[head-1] == 'x' || map[head+1] == 'x') head = (side-1)/2;
    }
    if (map[head+side] == 'x' || (head >= area-side && head <= area)) head = (side-1)/2;
}
void changeShapeRight()
{
    switch(figure)
    {
        case 2: figure = 5; map[last+1] = ' '; if(head%side == side-3) head++;break;
        case 3: figure = 12;map[last+1] = map[last-side+1] = map[last-side+2] = ' '; if(head%side == 0) head++; if(head%side == side-4) head+=2; break;
        case 4: figure = 14;map[last+1] = map[last+2] = map[last+3] = ' '; break;
        case 5: figure = 2; map[last-side] = ' '; if(head%side == side-2) head--; break;
        case 7: figure = 9; map[last+1] = ' '; break;
        case 8: figure = 7; map[last-1] = ' '; head--; break;
        case 9: figure = 10;map[last-side+1] = ' '; head++; break;
        case 10:figure = 8; map[last-side-1] = ' '; break;
        case 11:figure = 13;map[last-side] = map[last-side+1] = map[last-(2*side)+1] = ' '; if(head%side == 0) head+=2; if(head%side == side-3) head++; break;
        case 12:figure = 3; map[last-side] = map[last-side-1] = map[last-(2*side)-1] = ' '; if(head%side == 1) head--; if(head%side == side-2) head-=2; break;
        case 13:figure = 11;map[last-1] = map[last-side-1] = map[last-side-2] = ' '; if(head%side == 2)head-=2; if(head%side == side-2) head--; break;
        case 14:figure = 4; map[last-side] = map[last-(2*side)]= map[last-(3*side)] = ' '; if(head%side >= side-5) head -= head-side+5; break;
        case 15:figure = 17;map[last-1] = map[last+1] = ' '; if(head%side == 1) head--; break;
        case 16:figure = 18;map[last-side+1] = map[last-side-1] = ' '; break;
        case 17:figure = 16;map[last-(2*side)] = map[last-side+1] = ' '; if(head%side == 0) head++; break;
        case 18:figure = 15;map[last-(2*side)] = map[last-side-1] = ' '; if(head%side == 0) head++; if(head%side == side-2) head--; break;
    }
}
void changeShapeLeft()
{
    switch(figure)
    {
        case 2: figure = 5; map[last+1] = ' '; if(head%side == side-3) head++; break;
        case 3: figure = 13;map[last+1] = map[last-side+1] = map[last-side+2] = ' '; if(head%side == 0) head+=2; if(head%side == side-4) head+=2; break;
        case 4: figure = 14;map[last+1] = map[last+2] = map[last+3] = ' '; break;
        case 5: figure = 2; map[last-side] = ' '; if(head%side == side-2) head--; break;
        case 7: figure = 8; map[last+1] = ' '; head++;break;
        case 8: figure = 10;map[last-1] = ' '; break;
        case 9: figure = 7; map[last-side+1] = ' '; break;
        case 10:figure = 9; map[last-side-1] = ' '; head--;break;
        case 11:figure = 12;map[last-side] = map[last-side+1] = map[last-(2*side)+1] = ' '; if(head%side == 0) head++; if(head%side == side-3) head++; break;
        case 12:figure = 11;map[last-side] = map[last-side-1] = map[last-(2*side)-1] = ' '; if(head%side == 1) head--; if(head%side == side-2) head--; break;
        case 13:figure = 3; map[last-1] = map[last-side-1] = map[last-side-2] = ' '; if(head%side == 2)head-=2; if(head%side == side-2) head-=2; break;
        case 14:figure = 4; map[last-side] = map[last-(2*side)]= map[last-(3*side)] = ' '; if(head%side >= side-5) head -= head-side+5; break;
        case 15:figure = 18;map[last-1] = map[last+1] = ' ';  break;
        case 16:figure = 17;map[last-side+1] = map[last-side-1] = ' '; break;
        case 17:figure = 15;map[last-(2*side)] = map[last-side+1] = ' '; if(head%side == 0) head++; break;
        case 18:figure = 16;map[last-(2*side)] = map[last-side-1] = ' '; if(head%side == 0) head++; if(head%side == side-2) head--; break;
    }
}
void readValue() //inputting value from user
{   
    static struct termios oldt, newt;
    tcgetattr( STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON);
    tcsetattr( STDIN_FILENO, TCSANOW, &newt);
 
    char c = getchar();
    if (c == 'd' || c == 'a' || c == 'w' || c == 't' || c == 'q') value = c;
    tcsetattr( STDIN_FILENO, TCSANOW, &oldt);
    cv.notify_one();
}
void takeInput() //function to accept the value parallelly while game is proceeding.  
{
    thread th(readValue);
    mutex mtx;
    unique_lock<mutex> lck(mtx);
    while (cv.wait_for(lck, chrono::milliseconds(speed)) == cv_status::timeout) control();
    th.join();
    control();
}
void control() //Converts user input to the direction block must move
{
    last = head;
    convert();
    i = 0;
    hitWall = false;
    if (value == 'd')
    {
        i = 0;
        hitWall = false;
        while(i < v.size()){
            if(map[head+side+v[i]+1] == 'x' || head%side == side-block_length-1) hitWall = true;
            i++;
        }
        if(!hitWall) head++;
    }
    if (value == 'a'){
        i = 0;
        hitWall = false;
        while(i < u.size()){
            if(map[head+side+u[i]-2] == 'x' || head%side == block_width) hitWall = true;
            i++;
        }
        if(!hitWall) head--;
    }
    hitWall = false;
    if (value == 'w') changeShapeRight();
    if (value == 'q') changeShapeLeft();
    if (value == 't') {
        system("clear");
        gameToggle(false);
    }
    process();
}
void process()   //Brain of the program. Entire game operation happens here. 
{
    system("clear");
    if (map[head+side] == 'x' && head-(block_height)*side <= side) gameToggle(false);
    head += side;
    value = 'm';
    createShape();
    if (head == (side-1)/2) {
        destroyBlocks();
        listCounter++;
        convert();
    }
    display();
}
void display()
{
    blockPreview();
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
void gameToggle(bool toggle) 
{
    if(toggle) takeInput();
    else
    {
        fileManage(to_string(pace), 's');
        fileManage(to_string(score),'o');
        cout<<"Game Over!\nScore:"<<score;
        cin>>i;
    }
    gameToggle(true);
}
void mainMenu()   
{   
    char choice = 'z';
    cout<<"\nCreated by Allen\nPress:\n1 to Play\n2 for Help\n3 for Game Settings\n4 to exit\n";
    cin>>choice;
    system("clear");
    if(choice == '1') gameToggle(true);
    if(choice == '2')   //Instructions
    {   
        cout<<"CONTROLS\nPRESS\n a TO MOVE LEFT\n d TO MOVE RIGHT \n w TO ROTATE RIGHT \n q TO ROTATE LEFT";
        cin>>choice;
        system("clear");
        main();
    }
    if(choice == '3')
    {
        cout<<"Control the Block Speed. PRESS\n1 : Easy\n2 : Medium\n3 : Hard\n";
        cin>>pace;
        speedSelector();
        gameToggle(true);
    }
    else abort();
}
void fileManage(string data, char option)
{
    if(option == 'i')
    {
        ifstream fin("tetris_data.txt"); 
        if(!fin) cout<<"Welcome to the game!"; 
        else
        {
            string save_data;
            cout<<"Welcome back to the game!\nThe highscore is ";
            while (fin.good()) getline(fin,save_data);
            pace  = save_data[0] - '0';
            if (highscore < stoi(save_data.substr(2))) highscore = stoi(save_data.substr(2)) ;
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
        case 1: speed = 1000;break;
        case 3: speed = 600;break;
        default:speed = 800;pace = 2;break;
    }
}
