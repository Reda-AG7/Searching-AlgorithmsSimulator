#include "MainState.h"
#include<chrono>
#include<iostream>
#include "Components/Space.h"
#include<list>
#include<unordered_set>
#include<queue>

struct Compare{
    bool operator()(std::pair<int,int>& a, std::pair<int,int>& b){
        return a.first > b.first;
    }
};
using us_int = std::unordered_set<int>;
using pq = std::priority_queue<std::pair<int,int>, std::vector<std::pair<int,int>>,Compare>;
//*************** GLOBAL **************
//Fonts
sf::Font* titleFont = nullptr;
sf::Font* regularFont = nullptr;

//window && Background
sf::RenderWindow* window = nullptr;
sf::RectangleShape* background = nullptr;

int renderID = 0; // 0 : MainState, 1 : DFS, 2: BFS, 3: EXIT

std::chrono::steady_clock::time_point tStart,tEnd;
int startX = -1, startY = -1, targetX = -1, targetY = -1;
bool firstTimeRenderingAlgo = true;
bool startClicked = false;

int buttonID = 0; // for the buttons 0:back, 1:wall, 2:start, 3:target, 4:start
void initializeCoordinates() {
    startX = -1, startY = -1, targetX = -1, targetY = -1;
}
void initFonts(){
    //Fonts
    titleFont = new sf::Font();
    titleFont->loadFromFile("Fonts/Roboto-BoldItalic.ttf");

    regularFont = new sf::Font();
    regularFont->loadFromFile("Fonts/Roboto-Regular.ttf");
}
//Main State
int updateMainState(float& dt, MainState* mainState, sf::RenderWindow* window){
    sf::Vector2f mousePosition = sf::Vector2f((float)sf::Mouse::getPosition(*window).x,
                                            (float)sf::Mouse::getPosition(*window).y);
    return mainState->update(dt, mousePosition);
}
//For algorithms

//Initializers
std::vector<Button*> initButtons(sf::RenderWindow* window)
{
    std::vector<Button*> algoButtons(5);
    algoButtons[0] = new Button(100.0f,50.0f,150.0f,50.0f,"Back", *regularFont, 20u, sf::Color::White);
    algoButtons[1] = new Button(window->getSize().x/2 - 1.5*160.0f,130.0f,150.0f,50.0f,"Add Walls", *regularFont, 20u, sf::Color::White);
    algoButtons[2] = new Button(window->getSize().x/2 - 80,130.0f,150.0f,50.0f,"Add Start", *regularFont, 20u, sf::Color::White);
    algoButtons[3] = new Button(window->getSize().x/2 + 80.0f,130.0f,150.0f,50.0f,"Add Target", *regularFont, 20u, sf::Color::White);
    algoButtons[4] = new Button(window->getSize().x/2 + 1.5*160.0f,130.0f,150.0f,50.0f,"Start", *regularFont, 20u, sf::Color::White);
    return algoButtons;
}


sf::Text* initTime(sf::RenderWindow* window)
{
    sf::Text* gameTime = new sf::Text();
    gameTime->setString("00:00:00");
    gameTime->setFont(*regularFont);
    gameTime->setFillColor(sf::Color::Yellow);
    sf::FloatRect timeRec = gameTime->getGlobalBounds();
    gameTime->setOrigin(timeRec.left + timeRec.width/2.0f, timeRec.top + timeRec.height/2.0f);
    gameTime->setPosition(window->getSize().x-timeRec.width, 50.0f);
    return gameTime;;
}

void updateBoard(sf::RenderWindow* window, std::vector<std::vector<Space*>>& board)
{
    sf::Vector2f mousePosition = sf::Vector2f(static_cast<float>(sf::Mouse::getPosition(*window).x),
    static_cast<float>(sf::Mouse::getPosition(*window).y));
    float dt = 0;
    for(int i = 0;i<board.size();i++){
        for(int j=0;j<board[i].size();j++){
            board[i][j]->update(dt,mousePosition, buttonID);
            if(board[i][j]->isSpaceClicked()){
                if(buttonID == 2){
                    startX = i;
                    startY = j;
                }
                else if(buttonID == 3){
                    targetX = i;
                    targetY = j;
                }
            }
        }
    }
}
MyText* initTitle(sf::RenderWindow* window)
{
    std::string s;
    if(renderID == 1) s = "DFS";
    else if(renderID == 2) s = "BFS";
    else if(renderID == 3) s = "GBS (Heuristic)";
    MyText* title = new MyText(window->getSize().x/2.0f, 50.0f,s,*titleFont, 40u, sf::Color::White);
    return title;
}
void updateTime(sf::Text* time)
{
    tEnd = std::chrono::steady_clock::now();
    unsigned miliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(tEnd-tStart).count();
    unsigned seconds = miliseconds / 1000u;
    unsigned minutes = seconds/60u;
    miliseconds %= 99u;
    seconds %= 60u;
    std::string strMil = miliseconds > 9 ? std::to_string(miliseconds) : "0" + std::to_string(miliseconds);
    std::string strSec = seconds>9 ? std::to_string(seconds) : "0"+std::to_string(seconds);
    std::string strMin = minutes>9 ? std::to_string(minutes) : "0"+std::to_string(minutes);
    strMin += ((":"+strSec) + (":" + strMil));
    time->setString(strMin);
}
void updateButtons(std::vector<Button*>& buttons, sf::RenderWindow* window){
    sf::Vector2f mousePosition = sf::Vector2f((float)sf::Mouse::getPosition(*window).x,
                                            (float)sf::Mouse::getPosition(*window).y);
                                            float dt = 10.0f;
    for(auto& btn : buttons){
        btn->update(dt,mousePosition);
    }
    if(buttons[0]->isButtonClicked()){
        firstTimeRenderingAlgo = true;
        initializeCoordinates();
        buttonID= 0 ;
        renderID = 0;
    }
    //Buttons
    else if(buttons[1]->isButtonClicked()) buttonID = 1;
    else if(buttons[2]->isButtonClicked()) buttonID = 2;
    else if(buttons[3]->isButtonClicked()) buttonID = 3;
    else if(buttons[4]->isButtonClicked()) startClicked = true;
}
sf::RectangleShape* initFrame(sf::RenderWindow* window){
    sf::RectangleShape* boardFrame = new sf::RectangleShape(sf::Vector2f(window->getSize().x-10.0f,window->getSize().y-210.0f));
    boardFrame->setOutlineThickness(5u);
    boardFrame->setOutlineColor(sf::Color::Blue);
    boardFrame->setPosition(5.0f,200.0f);
    return boardFrame;
}
std::vector<std::vector<Space*>> initBoard(sf::RenderWindow* window, sf::RectangleShape* boardFrame)
{
    //Fill the board
    int y = boardFrame->getGlobalBounds().top+10;
    int width = boardFrame->getGlobalBounds().width;
    int x = boardFrame->getPosition().x+5;
    int height = boardFrame->getSize().y;
    int w = 20;
    auto board = std::vector<std::vector<Space*>>(height/w,std::vector<Space*>(width/w -1,nullptr));
    for(int i = 0;i<board.size();i++){
        for(int j=0;j<board[i].size();j++)
            board[i][j] = new Space(w*j+x,y+w*i,w);
    }
    return board;
}   


void render(sf::RenderWindow* window, sf::RectangleShape* background, std::vector<Button*>& buttons, sf::RectangleShape* boardFrame, std::vector<std::vector<Space*>>& board, sf::Text* time){
    //Updating
    if(startClicked) updateTime(time);
    updateButtons(buttons, window);
    updateBoard(window, board);
    
    auto title = initTitle(window);

    //Rendering
    window->clear();
    window->draw(*background);
    title->render(*window);
    window->draw(*time);
    for(auto& btn : buttons)
        btn->render(*window);
    window->draw(*boardFrame);
    for(auto& v: board){
        for(auto& i : v)
            i->render(*window);
    }
    window->display();
}
bool dfsAlgo(int i,int j,sf::RenderWindow* window, sf::RectangleShape* background, std::vector<Button*>& buttons, sf::RectangleShape* boardFrame, std::vector<std::vector<Space*>>& board, sf::Text* time){
    
    if(i<0 || i>=board.size() || j<0 || j>=board[0].size()) return false;
    if(i == targetX && j == targetY) return true;
    if(board[i][j]->getType() == 1) return false; // the wall
    if (board[i][j]->isVisited()) return false;
    board[i][j]->setVisited();
    if (i != startX || j != startY) board[i][j]->setType(true);
    int xm[4]{ -1,0,0,1 };
    int ym[4]{ 0,-1,1,0 };
    render(window,background,buttons,boardFrame,board,time);
    for (int a = 0; a < 4; a++) {
        int newI = i + xm[a];
        int newJ = j + ym[a];
        if (dfsAlgo(newI, newJ, window, background, buttons, boardFrame, board, time)) return true;
    }
    return false;
}
void dfs(sf::RenderWindow* window, sf::RectangleShape* background, std::vector<Button*>& buttons, sf::RectangleShape* boardFrame, std::vector<std::vector<Space*>>& board, sf::Text* time){
    if(startClicked){
        if (startX != -1 && startY != -1 && targetX != -1 && targetY != -1) {
            tStart = tEnd = std::chrono::steady_clock::now();
            int i = startX;
            int j = startY;
            dfsAlgo(i, j, window, background, buttons, boardFrame, board, time);
            startClicked = false;
        }
        else startClicked = false;
    }
    else render(window, background, buttons, boardFrame, board, time);
    
}
bool bfsAlgo(int i, int j, sf::RenderWindow* window, sf::RectangleShape* background, std::vector<Button*>& buttons, sf::RectangleShape* boardFrame, std::vector<std::vector<Space*>>& board, sf::Text* time) {
    std::list<std::pair<int, int>> q;
    q.push_back({ i,j });
    while (!q.empty()) {
        auto cur = q.front();
        q.pop_front();
        if (cur.first == targetX && cur.second == targetY) return true;
        if (cur.first != startX || cur.second != startY) board[cur.first][cur.second]->setType(true);
        board[cur.first][cur.second]->setVisited();
        if (cur.first + 1 < board.size() && !board[cur.first + 1][cur.second]->isVisited() && board[cur.first + 1][cur.second]->getType() != 1) {
            q.push_back({cur.first + 1 , cur.second});
            board[cur.first+1][cur.second]->setVisited();
            }
        if (cur.first - 1 >=0 && !board[cur.first - 1][cur.second]->isVisited() && board[cur.first - 1][cur.second]->getType() != 1) {
            q.push_back({ cur.first - 1 , cur.second });
            board[cur.first-1][cur.second]->setVisited();
            }
        if (cur.second + 1 < board[0].size() && !board[cur.first][cur.second + 1]->isVisited() && board[cur.first][cur.second + 1]->getType() != 1) {
            q.push_back({cur.first , cur.second+1});
            board[cur.first][cur.second + 1]->setVisited();
            }
        if (cur.second - 1 >= 0 && !board[cur.first][cur.second - 1]->isVisited() && board[cur.first][cur.second - 1]->getType() != 1) {
            q.push_back({ cur.first , cur.second - 1 });
            board[cur.first][cur.second - 1]->setVisited();
        }
        render(window, background, buttons, boardFrame, board, time);
    }
    return false;
}
void bfs(sf::RenderWindow* window, sf::RectangleShape* background, std::vector<Button*>& buttons, sf::RectangleShape* boardFrame, std::vector<std::vector<Space*>>& board, sf::Text* time) {
    if (startClicked) {
        if (startX != -1 && startY != -1 && targetX != -1 && targetY != -1) {
            tStart = tEnd = std::chrono::steady_clock::now();
            int i = startX;
            int j = startY;
            bfsAlgo(i, j, window, background, buttons, boardFrame, board, time);
            startClicked = false;
            initializeCoordinates();
        } else startClicked = false;
    }
    else render(window, background, buttons, boardFrame, board, time);

}
int manhattanDistance(int sX, int sY){
    return std::abs(targetX-sX) + std::abs(targetY-sY);
}
int getIndexFromCoordinates(int col, int i, int j){
    return i*col+j;
}
std::pair<int,int> getCoordinatesFromIndex(int& col, int index){
    int x = index/col;
    int y = index%col;
    return {x,y};
}
bool isValid(int i, int j, std::vector<std::vector<Space*>>& board, us_int& closedList){
    if(i<0 || i>=board.size() || j<0 || j>=board[0].size() || 
        board[i][j]->getType()==1) return false;
    int index = getIndexFromCoordinates(board[0].size(),i,j);
    if(closedList.find(index) != closedList.end()) return false;
    return true;
}
bool gbsAlgo(std::vector<std::pair<int,int>>& path, pq& opened_list,us_int& closed_list, sf::RenderWindow* window, sf::RectangleShape* background, std::vector<Button*>& buttons, sf::RectangleShape* boardFrame, std::vector<std::vector<Space*>>& board, sf::Text* time) {
    if(opened_list.size() == 0) return false;
    auto cur = opened_list.top();
    opened_list.pop();
    closed_list.emplace(cur.second);
    int c = board[0].size();
    auto p = getCoordinatesFromIndex(c,cur.second);
    //std::cout<<"Manhattan distance at ("<<p.first<<","<<p.second<<") index ("<<cur.second<<") = "<<cur.first<<std::endl;
    if(cur.first == 0){
        std::cout<<"Found it "<<std::endl;
        return true;
    }
    if(p.first != startX || p.second != startY) board[p.first][p.second]->setType(true);
    int x = p.first, y = p.second;
    if(isValid(x+1, y, board,closed_list)) opened_list.push({manhattanDistance(x+1,y),getIndexFromCoordinates(c,x+1,y)});
    if(isValid(x-1, y, board,closed_list)) opened_list.push({manhattanDistance(x-1,y),getIndexFromCoordinates(c,x-1,y)});
    if(isValid(x, y+1, board,closed_list)) opened_list.push({manhattanDistance(x,y+1),getIndexFromCoordinates(c,x,y+1)});
    if(isValid(x, y-1, board,closed_list)) opened_list.push({manhattanDistance(x,y-1),getIndexFromCoordinates(c,x,y-1)});
    if(gbsAlgo(path,opened_list,closed_list, window, background, buttons, boardFrame, board, time)){
        path.push_back(p);
        return true;
    }
    return false;
}
void gbs(sf::RenderWindow* window, sf::RectangleShape* background, std::vector<Button*>& buttons, sf::RectangleShape* boardFrame, std::vector<std::vector<Space*>>& board, sf::Text* time) {
    int r = board.size();
    int c = board[0].size();
    if (startClicked) {
        if (startX != -1 && startY != -1 && targetX != -1 && targetY != -1) {
            tStart = tEnd = std::chrono::steady_clock::now();
            us_int closedList;
            pq openedList;
            int mDistance = manhattanDistance(startX,startY);
            int index = getIndexFromCoordinates(c,startX, startY);
            openedList.push({mDistance,index});
            std::vector<std::pair<int,int>> path;
            gbsAlgo(path,openedList,closedList, window, background, buttons, boardFrame, board, time);
            startClicked = false;
            initializeCoordinates();
        }
        else startClicked = false;
    }
    else render(window, background, buttons, boardFrame, board, time);
}
void deleteBoard(std::vector<std::vector<Space*>>& board){
    for(auto i: board){
        for(auto& j: i) delete j;
    }
}
int main(){
    //window && background initialization
    window = new sf::RenderWindow(sf::VideoMode(1000,800),"Application", sf::Style::Close);
    window->setFramerateLimit(60);
    background = new sf::RectangleShape(sf::Vector2f(window->getSize().x, window->getSize().y));
    background->setFillColor(sf::Color(28, 40, 65));
    
    //Fonts initialization
    initFonts();

    //MainState Initialization
    MainState mainState(window, *titleFont, *regularFont);

    //AlgoState Data initialization
    sf::Clock clock;
    float dt;
    sf::RectangleShape* boardFrame = initFrame(window);
    std::vector<std::vector<Space*>> board = initBoard(window, boardFrame);
    auto algoButtons = initButtons(window);
    MyText* title = initTitle(window);
    sf::Text* time = initTime(window);

    //Work
    while(window->isOpen()){
        //Update Events
        sf::Event e;
        while(window->pollEvent(e)){
            if(e.type == sf::Event::Closed) 
                window->close();
        }

        
        switch(renderID){
        case 0: {
            renderID = updateMainState(dt, &mainState, window);
            time = initTime(window);
            window->clear();
            if (renderID == 4) {
                window->close();
                break;
            }
            mainState.render(*window);
            firstTimeRenderingAlgo = true;
            window->display();
        }; break;

        case 1: {
            //dfs
            if (firstTimeRenderingAlgo) {
                firstTimeRenderingAlgo = false;
                deleteBoard(board);
                board = initBoard(window, boardFrame);
            }
            dfs(window, background, algoButtons, boardFrame, board, time);

        }; break;
        case 2: {
            //bfs
            if (firstTimeRenderingAlgo) {
                firstTimeRenderingAlgo = false;
                deleteBoard(board);
                board = initBoard(window, boardFrame);
            }
            bfs(window, background, algoButtons, boardFrame, board, time);

        }; break;
        case 3: {
            //gbs
            if (firstTimeRenderingAlgo) {
                firstTimeRenderingAlgo = false;
                deleteBoard(board);
                board = initBoard(window, boardFrame);
            }
            gbs(window, background, algoButtons, boardFrame, board, time);

        }; break;

            default: break;
        }
        //deleteBoard(boardFrame, board);
        
        dt = clock.restart().asMilliseconds();
    }
    // Freeing the memory
    delete window;
    delete background;
    delete boardFrame;
    deleteBoard(board);
}