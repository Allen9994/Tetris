//Start of the program
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <cstdlib>
#include <termios.h>
#include <unistd.h>
#include <ctime>
#define  MAX_SHAPES 19

class Tetris {
private:
    short length, height, width, levelCounter, score, block;
    short figure, index, head, last, highscore, listCounter;
    short speed, pace, side, area, horz, vert, levelShift;
    bool hitWall;
    char input, value;
    std::vector<int> u, v, shapeList;
    std::string map, top, bottom, preview, saveFileName;

    std::mutex mtx;
    std::condition_variable cv;

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
    void fileManage(std::string data, char option);
    void speedSelector();
    void handleFileStatus();
    void gameToggle(bool toggle);
    void mainMenu();

    public:
        Tetris() 
                 : levelCounter(0), figure(0), levelShift(0), block(0), last(0),
                   score(0), highscore(0), listCounter(2), speed(1000), pace(2),
                   head(0), length(0), height(0), width(0), side(14), input(' '),
                   saveFileName("tetris_data.txt"), area(0), hitWall(false), value('m') {}
    void start() {
        fileManage("0", 'i');
        mainMenu();
    }
};

void Tetris::initialize() {
    shapeList.clear();
    area = side * side;
    head = (side - 1) / 2;
    map = std::string(area,' ');
    top = std::string(side + 1, '_');
    bottom = std::string(side + 1, '"');
    map = std::string(area,' ');
    srand((unsigned) time(0));
    for (index = 0; index < area; index++) shapeList.push_back((rand() % MAX_SHAPES) + 1);
    figure = shapeList[listCounter+1];
}

void Tetris::blockPreview() {
    switch (shapeList[listCounter]) { 
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
    std::cout<<preview<<std::endl;
}

void Tetris::destroyBlocks() {
    figure = shapeList[listCounter];
    levelCounter = area-1;
    while(levelCounter >= side) {
        if(map[levelCounter] == 'x') block++;
        if(block == side-1) { 
            score++;
            std::cout<<"\a";
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
    switch (figure) { 
        case 1:v = {0};                     u = {1};                            length=1;width=0;break;
        case 2:v = {1};                     u = {1};                            length=2;width=0;break;
        case 3:v = {1,-side+2};             u = {1,-side};                      length=3;width=0;break;
        case 4:v = {3};                     u = {1};                            length=4;width=0;break;
        case 5:v = {0,-side};               u = {1,-side-1};                    length=1;width=0;break;
        case 6:v = {1,-side+1};             u = {1,-side-1};                    length=2;width=0;break;
        case 7:v = {1,-side+1};             u = {1,-side-1};                    length=2;width=0;break;
        case 8:v = {0,-side};               u = {0,-side};                      length=1;width=1;break;
        case 9:v = {0,-side+1};             u = {1,-side-1};                    length=2;width=0;break;
        case 10:v= {0,-side};               u = {1,-side-2};                    length=1;width=1;break;
        case 11:v= {0,-side+1,(-2*side)+1}; u = {1,-side-1,-2*side};            length=2;width=0;break;
        case 12:v= {0,-side,-2*side};       u = {1,-side-1,-2*side-1};          length=1;width=1;break;
        case 13:v= {0};                     u = {0,-side-3};                    length=1;width=2;break;  
        case 14:v= {0,-side,2*-side,3*-side};u = {1,-side-1,-2*side-1,-3*side-1};length=1;width=0;break;
        case 15:v= {1,-side+1};             u = {0,-side};                      length=2;width=1;break;
        case 16:v= {0,-side+1};             u = {1,-side-2};                    length=2;width=1;break;
        case 17:v= {0,-side+1,2*side};      u = {1,-side-1,(2*side)-1};         length=2;width=0;break;
        case 18:v= {0,-side,2*side};        u = {1,-side-2,(2*side)-1};         length=1;width=1;break;
        case 19:v= {0,-side+1,2*side};      u = {1,-side-2,(2*side)-1};         length=2;width=1;break;
    }
}

void Tetris::createShape() {
    map[last] = ' ';
    map[head] = 'x';
    if (figure == 2) { 
        map[last+1] = ' ';
        map[head+1] = 'x';
        if (map[head+side+1] == 'x') head = (side-1)/2;
    }
    if (figure == 3) { 
        map[last+1] = map[last-side+1] = map[last-side+2] = ' ';
        map[head+1] = map[head-side+1] = map[head-side+2] = 'x';
        if (map[head+side+1] == 'x' || map[head+2] == 'x') head = (side-1)/2;
    }
    if (figure == 4) { 
        map[last+1] = map[last+2] = map[last+3] = ' ';
        map[head+1] = map[head+2] = map[head+3] = 'x';
        if (map[head+side+1] == 'x' || map[head+side+2] == 'x' || map[head+side+3] == 'x') head = (side-1)/2;
    }
    if (figure == 5) { 
        map[last-side] = ' ';
        map[head-side] = 'x';
    }
    if (figure == 6) { 
        map[last-side] = map[last+1] = map[last-side+1] = ' ';
        map[head-side] = map[head+1] = map[head-side+1] = 'x';
        if (map[head+side+1] == 'x') head = (side-1)/2;
    }
    if (figure == 7) { 
        map[last-side] = map[last+1] = ' ';
        map[head-side] = map[head+1] = 'x';
        if (map[head+side+1] == 'x') head = (side-1)/2;
    }
    if (figure == 8) { 
        map[last-side] = map[last-1] = ' ';
        map[head-side] = map[head-1] = 'x';
        if (map[head+side-1] == 'x') head = (side-1)/2;
    }
    if (figure == 9) { 
        map[last-side] = map[last-side+1] = ' ';
        map[head-side] = map[head-side+1] = 'x';
        if (map[head+1] == 'x') head = (side-1)/2;
    }
    if (figure == 10) { 
        map[last-side] = map[last-side-1] = ' ';
        map[head-side] = map[head-side-1] = 'x';
        if (map[head-1] == 'x') head = (side-1)/2;
    }
    if (figure == 11) { 
        map[last-side] = map[last-side+1] = map[last-(2*side)+1] = ' ';
        map[head-side] = map[head-side+1] = map[head-(2*side)+1] = 'x';
        if (map[head+1] == 'x') head = (side-1)/2;
    }
    if (figure == 12) { 
        map[last-side] = map[last-side-1] = map[last-(2*side)-1] = ' ';
        map[head-side] = map[head-side-1] = map[head-(2*side)-1] = 'x';
        if (map[head-1] == 'x') head = (side-1)/2;
    }
    if (figure == 13) { 
        map[last-1] = map[last-side-1] = map[last-side-2] = ' ';
        map[head-1] = map[head-side-1] = map[head-side-2] = 'x';
        if (map[head+side-1] == 'x' || map[head-2] == 'x') head = (side-1)/2;
    }
    if (figure == 14) { 
        map[last-side] = map[last-(2*side)]= map[last-(3*side)] = ' ';
        map[head-side] = map[head-(2*side)]= map[head-(3*side)] = 'x';
    }
    if (figure == 15) { 
        map[last-1] = map[last+1] = map[last-side] = ' ';
        map[head-1] = map[head+1] = map[head-side] = 'x';
        if (map[head+side+1] == 'x'|| map[head+side-1] == 'x') head = (side-1)/2;
    }
    if (figure == 16) { 
        map[last-side] = map[last-side+1] = map[last-side-1] = ' ';
        map[head-side] = map[head-side+1] = map[head-side-1] = 'x';
        if (map[head+1] == 'x'|| map[head-1] == 'x') head = (side-1)/2;
    }
    if (figure == 17) { 
        map[last-side] = map[last-(2*side)] = map[last-side+1] = ' ';
        map[head-side] = map[head-(2*side)] = map[head-side+1] = 'x';
        if (map[head+1] == 'x') head = (side-1)/2;
    }
    if (figure == 18) { 
        map[last-side] = map[last-(2*side)] = map[last-side-1] = ' ';
        map[head-side] = map[head-(2*side)] = map[head-side-1] = 'x';
        if (map[head-1] == 'x') head = (side-1)/2;
    }
    if (figure == 19) { 
        map[last-side] = map[last-(2*side)] = map[last-side-1] = map[last-side+1] = ' ';
        map[head-side] = map[head-(2*side)] = map[head-side-1] = map[head-side+1] = 'x';
        if (map[head-1] == 'x' || map[head+1] == 'x') head = (side-1)/2;
    }
    if (map[head+side] == 'x' || (head >= area-side && head <= area)) head = (side-1)/2;
}

void Tetris::changeShapeRight() {
    switch (figure) { 
        case 2: figure = 5; map[last+1] = ' '; if(head%side == side-3) head++; break;
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
    switch(figure) { 
        case 2: figure = 5; map[last+1] = ' '; if(head%side == side-3) head++; break;
        case 3: figure = 13;map[last+1] = map[last-side+1] = map[last-side+2] = ' '; if(head%side == 0) head+=2; if(head%side == side-4) head+=2; break;
        case 4: figure = 14;map[last+1] = map[last+2] = map[last+3] = ' '; break;
        case 5: figure = 2; map[last-side] = ' '; if(head%side == side-2) head--; break;
        case 7: figure = 8; map[last+1] = ' '; head++; break;
        case 8: figure = 10;map[last-1] = ' '; break;
        case 9: figure = 7; map[last-side+1] = ' '; break;
        case 10:figure = 9; map[last-side-1] = ' '; head--; break;
        case 11:figure = 12;map[last-side] = map[last-side+1] = map[last-(2*side)+1] = ' '; if(head%side == 0) head++; if(head%side == side-3) head++; break;
        case 12:figure = 11;map[last-side] = map[last-side-1] = map[last-(2*side)-1] = ' '; if(head%side == 1) head--; if(head%side == side-2) head--; break;
        case 13:figure = 3; map[last-1] = map[last-side-1] = map[last-side-2] = ' '; if(head%side == 2)head-=2; if(head%side == side-2) head-=2; break;
        case 14:figure = 4; map[last-side] = map[last-(2*side)]= map[last-(3*side)] = ' '; if(head%side >= side-5) head -= head-side+5; break;
        case 15:figure = 18;map[last-1] = map[last+1] = ' '; break;
        case 16:figure = 17;map[last-side+1] = map[last-side-1] = ' '; if(head%side == 1) head--; break;
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
    input = getchar();
    if (input == 'd' || input == 'a' || input == 'w' || input == 't' || input == 'q') value = input;
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    cv.notify_one();
}

void Tetris::takeInput() {
    std::thread th(&Tetris::readValue, this);
    std::unique_lock<std::mutex> lck(mtx);
    while (cv.wait_for(lck, std::chrono::milliseconds(speed)) == std::cv_status::timeout) gameControl();
    th.join();
    gameControl();
}

void Tetris::gameControl() {
    last = head;
    convertShape();
    hitWall = false;
    height = v.size()-1;
    switch (value) {
        case 'd' :
            for (index = 0; index <= height; index++) if(map[head+side+v[index]+1] == 'x' || head%side == side-length-1) hitWall = true;
            head += !hitWall;
            break;
        case 'a' :
            for (index = 0; index <= height; index++) if(map[head+side+u[index]-2] == 'x' || head%side == width) hitWall = true;
            head -= !hitWall;
            break;
        case 'w': changeShapeRight();   break;
        case 'q': changeShapeLeft();    break;
        case 't': gameToggle(false);    break;
    }
    gameAlgorithm();
}

void Tetris::gameAlgorithm() {
    if (map[head+side] == 'x' && head - (height * side) <= side) gameToggle(false);
    system("clear");
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
    std::cout << top << std::endl;
    for (vert = 0; vert < side; vert++) {
        for (horz = 0; horz < side; horz++) {
            if (horz == side - 1 || horz == 0) std::cout << "|";
            if (horz == side - 1 && vert  == side - 1) std::cout << std::endl << bottom;
            std::cout << map[(vert*side)+horz];
        }
        std::cout << std::endl;
    }
}

void Tetris::fileManage(std::string data, char option) {
    if (option == 'i') { 
        std::ifstream fin(saveFileName); 
        if (!fin) std::cout << "Welcome to Tetris!"; 
        else {
            std::string save_data;
            while (fin.good()) getline(fin,save_data);
            if (save_data.size() < 4 || save_data.size() > 7 || !all_of(save_data.begin(), save_data.end(), ::isdigit)) {
                handleFileStatus();
                fin.close();
                abort();
            }
            pace = save_data[0] - '0';
            side = save_data[1] - '0';
            side*= 10;
            side+= save_data[2] - '0';
            std::cout << "Welcome back to Tetris!\nThe highscore is ";
            if (highscore < stoi(save_data.substr(3))) highscore = stoi(save_data.substr(3));
            std::cout << highscore;
            speedSelector();
        }
        initialize();
        fin.close();
    }
    else if (option == 's') {
        std::ofstream fout(saveFileName,std::ios::app);
        fout << std::endl << data;
        fout.close();
    }
    else if (option == 'o') {
        std::ofstream fout(saveFileName, std::ios::app);
        if(stoi(data) > highscore) { 
            highscore = stoi(data);
            std::cout << "HIGHSCORE! "<<highscore<<std::endl;
        }
        fout << highscore;
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

void Tetris::handleFileStatus() {
    std::cout << "The save file is corrupted! \nKindly restart the game as the save file is reset\n";
    std::ofstream fout(saveFileName, std::ios::app);
    fout << std::endl << "2120";
    fout.close();
    sleep(1);
    std::cout << "Terminating..\n";
}

void Tetris::gameToggle(bool toggle) {
    if (toggle) takeInput();
    else {
        system("clear");
        fileManage(std::to_string(pace) + std::to_string(side), 's');
        fileManage(std::to_string(score * pace), 'o');
        std::cout << "Game Over!\nScore:" << score * pace << "\n";
        abort();
    }
    gameToggle(true);
}

void Tetris::mainMenu() {
    char choice = 'z';
    std::cout << "\nPress:\n\t1 to Play\n\t2 for Help\n\t3 for Game Settings\n\t4 to Exit\n";
    std::cin >> choice;
    system("clear");
    if (choice == '1') gameToggle(true);
    if (choice == '2') {
        std::cout << "PRESS\n \ta TO MOVE LEFT\n \td TO MOVE RIGHT \n \tw TO ROTATE RIGHT \n \tq TO ROTATE LEFT\n \tt TO QUIT GAME \n";
        sleep(3);
        system("clear");
        mainMenu();
    }
    if (choice == '3') {
        std::string value_entered;
        std::cout << "Control the Block Speed. PRESS\n1 : Easy\n2 : Medium\n3 : Hard\n";
        std::cin >> value_entered;
        if(all_of(value_entered.begin(),value_entered.end(), ::isdigit)) pace = stoi(value_entered);
        speedSelector();
        std::cout << "Enter the map size of range[10-15]\n";
        std::cin >> value_entered;
        if(all_of(value_entered.begin(),value_entered.end(), ::isdigit)) {
            short num = stoi(value_entered);
            if(num > 9 && num < 16) {
                side = num;
                initialize();
            }
            else std::cout<<"Size entered not within range!\nReverting to previous size...\n";
            sleep(1);
        }
        system("clear");
        mainMenu();
    }
    else abort();
}

int main() {
    Tetris game;
    game.start();
    return 0;
}
//End of the program
