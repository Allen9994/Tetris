//Start of program
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

class Tetris {
private:
    int levelCounter, score, figure, levelShift, block;
    int last, highscore, previewFigure, listCounter;
    int i, speed, pace, head, side, area;
    int block_length, block_height, block_width;
    bool hitWall;
    char value;
    vector<int> u, v, shapeList;
    string map, uline, bline, preview;

    mutex mtx;
    condition_variable cv;

    void blockPreview();
    void initialize();
    void destroyBlocks();
    void convertShape();
    void createShape();
    void changeShapeRight();
    void changeShapeLeft();
    void readValue();
    void takeInput();
    void gameControl();
    void gameAlgorithm();
    void gameDisplay();
    void fileManage(string data, char option);
    void speedSelector();
    void checkFileStatus();
    void gameToggle(bool toggle);
    void mainMenu();

    public:
        Tetris(int size_map) 
                 : levelCounter(0), score(0), figure(0), levelShift(0), block(0), last(0),
                   highscore(0), previewFigure(0), listCounter(2), speed(1000), pace(2),
                   head(0), block_length(0), block_height(0), block_width(0), side(size_map),
                   area(0), hitWall(false), value('m') {}
    void start() {
        fileManage("0", 'i');
        mainMenu();
    }
};

void Tetris::initialize() {
    shapeList.clear();
    area = side * side;
    head = (side - 1) / 2;
    map = string(area, ' ');
    uline = string(side + 1, '_');
    bline = string(side + 1, '"');
    map = string(area,' ');
    srand((unsigned) time(0));
    for (int index = 0; index < area; index++) shapeList.push_back((rand() % 19) + 1);
    figure = shapeList[listCounter];
}

void Tetris::blockPreview() {
    previewFigure = shapeList[listCounter];
    switch(previewFigure){
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

void Tetris::destroyBlocks() {
    figure = shapeList[listCounter];
    levelCounter = area-1;
    while(levelCounter >= side) {
        if(map[levelCounter] == 'x') block++;
        if(block == side-1){
            score++;
            levelShift = levelCounter-1;
            while(levelShift >= 0) {
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

void Tetris::convertShape() {
    v.clear();
    u.clear();
    switch (figure){
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

void Tetris::createShape() {
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

void Tetris::changeShapeRight() {
    switch(figure){
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
        case 16:figure = 18;map[last-side+1] = map[last-side-1] = ' '; head++; break;
        case 17:figure = 16;map[last-(2*side)] = map[last-side+1] = ' '; if(head%side == 0) head++; break;
        case 18:figure = 15;map[last-(2*side)] = map[last-side-1] = ' '; if(head%side == 0) head++; if(head%side == side-2) head--; break;
    }
}

void Tetris::changeShapeLeft() {
    switch(figure){
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

void Tetris::readValue() {
    static struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    char c = getchar();
    if (c == 'd' || c == 'a' || c == 'w' || c == 't' || c == 'q') value = c;
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    cv.notify_one();
}

void Tetris::takeInput() {
    thread th(&Tetris::readValue, this);
    unique_lock<mutex> lck(mtx);
    while (cv.wait_for(lck, chrono::milliseconds(speed)) == cv_status::timeout) gameControl();
    th.join();
    gameControl();
}

void Tetris::gameControl() {
    last = head;
    convertShape();
    i = 0;
    hitWall = false;
    if (value == 'd') {
        hitWall = false;
        for(i = 0; i < v.size(); i++) {
            if(map[head+side+v[i]+1] == 'x' || head%side == side-block_length-1) hitWall = true;
        }
        if(!hitWall) head++;
    }
    else if (value == 'a') {
        hitWall = false;
        for(i = 0; i < u.size(); i++) {
            if(map[head+side+u[i]-2] == 'x' || head%side == block_width) hitWall = true;
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
    gameAlgorithm();
}

void Tetris::gameAlgorithm() {
    system("clear");
    if (map[head + side] == 'x' && head - (block_height) * side <= side) gameToggle(false);
    head += side;
    value = 'm';
    createShape();
    if (head == (side - 1) / 2) {
        destroyBlocks();
        listCounter++;
        convertShape();
    }
    gameDisplay();
}

void Tetris::gameDisplay() {
    blockPreview();
    cout << uline << endl;
    for (int j = 0; j < side; j++) {
        for (int i = 0; i < side; i++) {
            if (i == side - 1 || i == 0) cout << "|";
            if (i == side - 1 && j == side - 1) cout << endl << bline;
            cout << map[(j * side) + i];
        }
        cout << endl;
    }
}

void Tetris::fileManage(string data, char option) {
    if(option == 'i'){
        ifstream fin("tetris_data.txt"); 
        if(!fin) {
            cout<<"Welcome to Tetris!"; 
            initialize();
        }
        else{
            string save_data;
            while (fin.good()) getline(fin,save_data);
            if(save_data.size() <= 3 || !all_of(save_data.begin(), save_data.end(), ::isdigit)) {
                checkFileStatus();
                fin.close();
                abort();
            }
            pace = save_data[0] - '0';
            side = save_data[1] - '0';
            side*= 10;
            side+= save_data[2] - '0';
            initialize();
            cout<<"Welcome back to Tetris!\nThe highscore is ";
            if (highscore < stoi(save_data.substr(3))) highscore = stoi(save_data.substr(3)) ;
            cout<<highscore;
            speedSelector();
        }
        fin.close();
    }
    else if(option == 's'){
        ofstream fout("tetris_data.txt",ios::app);
        fout<<endl<<data;
        fout.close();
    }
    else if(option == 'o'){
        ofstream fout("tetris_data.txt", ios::app);
        if(stoi(data) > highscore){
            highscore = stoi(data);
            cout<<"HIGHSCORE! "<<highscore<<endl;
        }
        fout<<highscore;
        fout.close();
    }
}

void Tetris::speedSelector() {
    switch (pace) {
        case 1: speed = 1000; break;
        case 3: speed = 600; break;
        default: speed = 800; pace = 2; break;
    }
}

void Tetris::checkFileStatus() {
    cout << "The save file is corrupted! \nKindly restart the game as the save file is reset\n";
    ofstream fout("tetris_data.txt", ios::app);
    fout << endl << "2120";
    fout.close();
    sleep(1);
    cout << "Terminating..\n";
}

void Tetris::gameToggle(bool toggle) {
    if (toggle) takeInput();
    else {
        fileManage(to_string(pace) + to_string(side), 's');
        fileManage(to_string(score * pace), 'o');
        cout << "Game Over!\nScore:" << score * pace << "\n";
        abort();
    }
    gameToggle(true);
}

void Tetris::mainMenu() {
    char choice = 'z';
    cout << "\nPress:\n\t1 to Play\n\t2 for Help\n\t3 for Game Settings\n\t4 to Exit\n";
    cin >> choice;
    system("clear");
    if (choice == '1') gameToggle(true);
    if (choice == '2') {
        cout << "gameControlS\nPRESS\n \ta TO MOVE LEFT\n \td TO MOVE RIGHT \n \tw TO ROTATE RIGHT \n \tq TO ROTATE LEFT";
        cin >> choice;
        system("clear");
        mainMenu();
    }
    if (choice == '3') {
        cout << "gameControl the Block Speed. PRESS\n1 : Easy\n2 : Medium\n3 : Hard\n";
        cin >> pace;
        speedSelector();
        cout << "Enter the map size of range[10-15]\n";
        string size_entered;
        cin>>size_entered;
        if(all_of(size_entered.begin(),size_entered.end(),::isdigit)) {
            int num = stoi(size_entered);
            if(num > 9 && num < 16) {
                side = num;
            }
            else cout<<"Size entered not within range!\n Reverting to previous size...\n";
            sleep(3);
            initialize();
        }
        gameToggle(true);
    }
    else abort();
}

int main() {
    Tetris game(14);
    game.start();
    return 0;
}
//End of program
