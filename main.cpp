#include "MainState.h"
#include<chrono>
#include<iostream>
#include "Components/Space.h"
#include<list>
#include<unordered_set>
#include<queue>


//*************** GLOBAL **************
//using functor to compare elements in pq
struct Compare {
    bool operator()(const std::pair<int, int>& a, const std::pair<int, int>& b) {
        return a.first > b.first;
    }
};

using us_int = std::unordered_set<int>;
using pq = std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, Compare>;
//using lambda to compare elements in pq
auto Compare2 = [](Space* a, Space* b) -> bool {return a->getF() > b->getF(); };
using pq2 = std::priority_queue < Space*, std::vector<Space*>, decltype(Compare2)>;
//Fonts
sf::Font* titleFont = nullptr;
sf::Font* regularFont = nullptr;

//window && Background
sf::RenderWindow* window = nullptr;
sf::RectangleShape* background = nullptr;
sf::RectangleShape* boardFrame = nullptr;

int renderID = 0; // 0 : MainState, 1 : DFS, 2: BFS, 3: EXIT

std::chrono::steady_clock::time_point tStart, tEnd;
int startX = -1, startY = -1, targetX = -1, targetY = -1;
bool firstTimeRenderingAlgo = true;
bool startClicked = false;

int buttonID = 0; // for the buttons 0:back, 1:wall, 2:start, 3:target, 4:start

//Initializers
void initWindowAndBackground() {
    //window && background initialization
    window = new sf::RenderWindow(sf::VideoMode(1000, 800), "Application", sf::Style::Close);
    window->setFramerateLimit(60);
    background = new sf::RectangleShape(sf::Vector2f(window->getSize().x, window->getSize().y));
    background->setFillColor(sf::Color(28, 40, 65));
}
void initializeCoordinates() {
    startX = -1, startY = -1, targetX = -1, targetY = -1;
}
void initFonts() {
    //Fonts
    titleFont = new sf::Font();
    titleFont->loadFromFile("Fonts/Roboto-BoldItalic.ttf");

    regularFont = new sf::Font();
    regularFont->loadFromFile("Fonts/Roboto-Regular.ttf");
}
std::vector<Button*> initButtons()
{
    std::vector<Button*> algoButtons(5);
    algoButtons[0] = new Button(100.0f, 50.0f, 150.0f, 50.0f, "Back", *regularFont, 20u, sf::Color::White);
    algoButtons[1] = new Button(window->getSize().x / 2 - 1.5 * 160.0f, 130.0f, 150.0f, 50.0f, "Add Walls", *regularFont, 20u, sf::Color::White);
    algoButtons[2] = new Button(window->getSize().x / 2 - 80, 130.0f, 150.0f, 50.0f, "Add Start", *regularFont, 20u, sf::Color::White);
    algoButtons[3] = new Button(window->getSize().x / 2 + 80.0f, 130.0f, 150.0f, 50.0f, "Add Target", *regularFont, 20u, sf::Color::White);
    algoButtons[4] = new Button(window->getSize().x / 2 + 1.5 * 160.0f, 130.0f, 150.0f, 50.0f, "Start", *regularFont, 20u, sf::Color::White);
    return algoButtons;
}
sf::Text* initTime()
{
    sf::Text* gameTime = new sf::Text();
    gameTime->setString("00:00:00");
    gameTime->setFont(*regularFont);
    gameTime->setFillColor(sf::Color::Yellow);
    sf::FloatRect timeRec = gameTime->getGlobalBounds();
    gameTime->setOrigin(timeRec.left + timeRec.width / 2.0f, timeRec.top + timeRec.height / 2.0f);
    gameTime->setPosition(window->getSize().x - timeRec.width, 50.0f);
    return gameTime;;
}
MyText* initTitle()
{
    std::string s;
    if (renderID == 1) s = "DFS";
    else if (renderID == 2) s = "BFS";
    else if (renderID == 3) s = "GBS (Heuristic)";
    else if (renderID == 5) s = "Dijkstra";
    MyText* title = new MyText(window->getSize().x / 2.0f, 50.0f, s, *titleFont, 40u, sf::Color::White);
    return title;
}
sf::RectangleShape* initFrame() {
    sf::RectangleShape* boardFrame = new sf::RectangleShape(sf::Vector2f(window->getSize().x - 10.0f, window->getSize().y - 210.0f));
    boardFrame->setOutlineThickness(5u);
    boardFrame->setOutlineColor(sf::Color::Blue);
    boardFrame->setPosition(5.0f, 200.0f);
    return boardFrame;
}
std::vector<std::vector<Space*>> initBoard()
{
    //Fill the board
    int y = boardFrame->getGlobalBounds().top + 10;
    int width = boardFrame->getGlobalBounds().width;
    int x = boardFrame->getPosition().x + 5;
    int height = boardFrame->getSize().y;
    int w = 20;
    auto board = std::vector<std::vector<Space*>>(height / w, std::vector<Space*>(width / w - 1, nullptr));
    for (int i = 0; i < board.size(); i++) {
        for (int j = 0; j < board[i].size(); j++)
            board[i][j] = new Space(w * j + x, y + w * i, w,i,j);
    }
    return board;
}


//Updaters
void updateBoard(std::vector<std::vector<Space*>>& board)
{
    sf::Vector2f mousePosition = sf::Vector2f(static_cast<float>(sf::Mouse::getPosition(*window).x),
        static_cast<float>(sf::Mouse::getPosition(*window).y));
    float dt = 0;
    for (int i = 0; i < board.size(); i++) {
        for (int j = 0; j < board[i].size(); j++) {
            board[i][j]->update(dt, mousePosition, buttonID);
            if (board[i][j]->isSpaceClicked()) {
                if (buttonID == 2) {
                    startX = i;
                    startY = j;
                }
                else if (buttonID == 3) {
                    targetX = i;
                    targetY = j;
                }
            }
        }
    }
}
void updateTime(sf::Text* time)
{
    tEnd = std::chrono::steady_clock::now();
    unsigned miliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(tEnd - tStart).count();
    unsigned seconds = miliseconds / 1000u;
    unsigned minutes = seconds / 60u;
    miliseconds %= 99u;
    seconds %= 60u;
    std::string strMil = miliseconds > 9 ? std::to_string(miliseconds) : "0" + std::to_string(miliseconds);
    std::string strSec = seconds > 9 ? std::to_string(seconds) : "0" + std::to_string(seconds);
    std::string strMin = minutes > 9 ? std::to_string(minutes) : "0" + std::to_string(minutes);
    strMin += ((":" + strSec) + (":" + strMil));
    time->setString(strMin);
}
void updateButtons(std::vector<Button*>& buttons) {
    sf::Vector2f mousePosition = sf::Vector2f((float)sf::Mouse::getPosition(*window).x,
        (float)sf::Mouse::getPosition(*window).y);
    float dt = 10.0f;
    for (auto& btn : buttons) {
        btn->update(dt, mousePosition);
    }
    if (buttons[0]->isButtonClicked()) {
        firstTimeRenderingAlgo = true;
        initializeCoordinates();
        buttonID = 0;
        renderID = 0;
    }
    //Buttons
    else if (buttons[1]->isButtonClicked()) buttonID = 1;
    else if (buttons[2]->isButtonClicked()) buttonID = 2;
    else if (buttons[3]->isButtonClicked()) buttonID = 3;
    else if (buttons[4]->isButtonClicked()) startClicked = true;
}
int updateMainState(float& dt, MainState* mainState) {
    sf::Vector2f mousePosition = sf::Vector2f((float)sf::Mouse::getPosition(*window).x,
        (float)sf::Mouse::getPosition(*window).y);
    return mainState->update(dt, mousePosition);
}

//Renderers
void render(std::vector<Button*>& buttons, std::vector<std::vector<Space*>>& board, sf::Text* time) {
    //Updating
    if (startClicked) updateTime(time);
    updateButtons(buttons);
    updateBoard(board);

    auto title = initTitle();

    //Rendering
    window->clear();
    window->draw(*background);
    title->render(*window);
    window->draw(*time);
    for (auto& btn : buttons)
        btn->render(*window);
    window->draw(*boardFrame);
    for (auto& v : board) {
        for (auto& i : v)
            i->render(*window);
    }
    window->display();
}

//Helpers
int manhattanDistance(int sX, int sY) {
    return std::abs(targetX - sX) + std::abs(targetY - sY);
}
int getIndexFromCoordinates(int col, int i, int j) {
    return i * col + j;
}
std::pair<int, int> getCoordinatesFromIndex(int& col, int index) {
    int x = index / col;
    int y = index % col;
    return { x,y };
}
bool isValid(int i, int j, std::vector<std::vector<Space*>>& board, us_int& closedList) {
    if (i < 0 || i >= board.size() || j < 0 || j >= board[0].size() ||
        board[i][j]->getType() == 1) return false;
    int index = getIndexFromCoordinates(board[0].size(), i, j);
    if (closedList.find(index) != closedList.end()) return false;
    return true;
}
bool isValid2(int i, int j, std::vector<std::vector<Space*>>& board, std::unordered_set<Space*>& visited) {
    if (i < 0 || i >= board.size() || j < 0 || j >= board[0].size() ||
        board[i][j]->getType() == 1) return false;
    if (visited.find(board[i][j]) != visited.end()) return false;
    return true;
}

void drawPath(Space* cur) {
    cur->setPath();
    while (cur->getParent()) {
        cur->setPath();
        cur = cur->getParent();
    }
}

//Algorithms
bool dfsAlgo(int i, int j, std::vector<Button*>& buttons, std::vector<std::vector<Space*>>& board, sf::Text* time, std::vector<std::pair<int,int>>& path) {

    if (i < 0 || i >= board.size() || j < 0 || j >= board[0].size()) return false;
    if (i == targetX && j == targetY) return true;
    if (board[i][j]->getType() == 1) return false; // the wall
    if (board[i][j]->isVisited()) return false;
    board[i][j]->setVisited();
    if (i != startX || j != startY) board[i][j]->setType(true);
    int xm[4]{ -1,0,0,1 };
    int ym[4]{ 0,-1,1,0 };
    render(buttons, board, time);
    for (int a = 0; a < 4; a++) {
        int newI = i + xm[a];
        int newJ = j + ym[a];
        if (dfsAlgo(newI, newJ, buttons, board, time, path)) {
            path.push_back({ i, j });
            return true;
        }
    }
    return false;
}
void dfs(std::vector<Button*>& buttons, std::vector<std::vector<Space*>>& board, sf::Text* time) {
    if (startClicked) {
        if (startX != -1 && startY != -1 && targetX != -1 && targetY != -1) {
            tStart = tEnd = std::chrono::steady_clock::now();
            int i = startX;
            int j = startY;
            std::vector<std::pair<int, int>> path;
            dfsAlgo(i, j, buttons, board, time, path);
            if (path.size() > 0) {
                for (int i = 0; i < path.size() - 1;++i) 
                    board[path[i].first][path[i].second]->setPath(); 
            }
            startClicked = false;
        }
        else startClicked = false;
    }
    else render(buttons, board, time);

}
bool bfsAlgo(int i, int j, std::vector<Button*>& buttons, std::vector<std::vector<Space*>>& board, sf::Text* time) {
    std::list<std::pair<int, int>> q;
    q.push_back({ i,j });
    while (!q.empty()) {
        auto cur = q.front();
        q.pop_front();
        if (cur.first == targetX && cur.second == targetY) return true;
        if (cur.first != startX || cur.second != startY) board[cur.first][cur.second]->setType(true);
        board[cur.first][cur.second]->setVisited();
        if (cur.first + 1 < board.size() && !board[cur.first + 1][cur.second]->isVisited() && board[cur.first + 1][cur.second]->getType() != 1) {
            q.push_back({ cur.first + 1 , cur.second });
            board[cur.first + 1][cur.second]->setVisited();
            board[cur.first + 1][cur.second]->setParent(*board[cur.first][cur.second]);
        }
        if (cur.first - 1 >= 0 && !board[cur.first - 1][cur.second]->isVisited() && board[cur.first - 1][cur.second]->getType() != 1) {
            q.push_back({ cur.first - 1 , cur.second });
            board[cur.first - 1][cur.second]->setVisited();
            board[cur.first - 1][cur.second]->setParent(*board[cur.first][cur.second]);
        }
        if (cur.second + 1 < board[0].size() && !board[cur.first][cur.second + 1]->isVisited() && board[cur.first][cur.second + 1]->getType() != 1) {
            q.push_back({ cur.first , cur.second + 1 });
            board[cur.first][cur.second + 1]->setVisited();
            board[cur.first][cur.second + 1]->setParent(*board[cur.first][cur.second]);
        }
        if (cur.second - 1 >= 0 && !board[cur.first][cur.second - 1]->isVisited() && board[cur.first][cur.second - 1]->getType() != 1) {
            q.push_back({ cur.first , cur.second - 1 });
            board[cur.first][cur.second - 1]->setVisited();
            board[cur.first][cur.second - 1]->setParent(*board[cur.first][cur.second]);
        }
        render(buttons, board, time);
    }
    return false;
}
void bfs(std::vector<Button*>& buttons, std::vector<std::vector<Space*>>& board, sf::Text* time) {
    if (startClicked) {
        if (startX != -1 && startY != -1 && targetX != -1 && targetY != -1) {
            tStart = tEnd = std::chrono::steady_clock::now();
            int i = startX;
            int j = startY;
            bfsAlgo(i, j, buttons, board, time);
            //Draw path
            Space* x = board[targetX][targetY]->getParent();
            if (x) drawPath(x);

            startClicked = false;
            initializeCoordinates();
        }
        else startClicked = false;
    }
    else render(buttons, board, time);

}
bool gbsAlgo(pq& opened_list, us_int& closed_list, std::vector<Button*>& buttons, std::vector<std::vector<Space*>>& board, sf::Text* time) {
    while (!opened_list.empty()) {
        auto cur = opened_list.top();
        opened_list.pop();
        int mDistance = cur.first;
        int index = cur.second;
        if (mDistance == 0) return true;
        if (closed_list.find(index) != closed_list.end()) continue;
        closed_list.emplace(index);

        int c = board[0].size();
        auto p = getCoordinatesFromIndex(c, cur.second);
        int x = p.first, y = p.second;
        if (x != startX || y != startY)  board[x][y]->setType(true);
        
        render(buttons, board, time);
        int xm[4]{ -1,0,0,1 };
        int ym[4]{ 0,-1,1,0 };
        for (int i = 0; i < 4; i++) {
            int newX = x + xm[i];
            int newY = y + ym[i];
            if (isValid(newX, newY, board, closed_list)) {
                int m_dist = manhattanDistance(newX, newY);
                int idx = getIndexFromCoordinates(c, newX, newY);
                board[newX][newY]->setParent(*board[x][y]);
                opened_list.push({ m_dist, idx });
            }
        }
    }
    return false;
}
void gbs(std::vector<Button*>& buttons, std::vector<std::vector<Space*>>& board, sf::Text* time) {
    int c = board[0].size();
    if (startClicked) {
        if (startX != -1 && startY != -1 && targetX != -1 && targetY != -1) {
            tStart = tEnd = std::chrono::steady_clock::now();
            us_int closedList;
            pq openedList;
            int mDistance = manhattanDistance(startX, startY);
            int index = getIndexFromCoordinates(c, startX, startY);
            openedList.push({ mDistance,index });
            gbsAlgo(openedList, closedList, buttons, board, time);
            Space* x = board[targetX][targetY]->getParent();
            if (x) drawPath(x);
            startClicked = false;
            initializeCoordinates();
        }
        else startClicked = false;
    }
    else render(buttons, board, time);
}
bool dijkstraAlgo(pq2& opened_list, std::unordered_set<Space*>& visited, std::vector<Button*>& buttons, std::vector<std::vector<Space*>>& board, sf::Text* time) {
    bool res = false;
    while (!opened_list.empty()) {
        Space* cur = opened_list.top();
        opened_list.pop();
        if (visited.find(cur) != visited.end()) continue;
        visited.emplace(cur);
        auto [x,y] = cur->getCoordinates();
        render(buttons, board, time);
        int xm[4]{ -1,0,0,1 };
        int ym[4]{ 0,-1,1,0 };
        for (int a = 0; a < 4; a++) {
            int newI = x + xm[a];
            int newJ = y + ym[a];
            if (isValid2(newI, newJ, board, visited)) {
                if(cur->getF() + 1 < board[newI][newJ]->getF()){
                    board[newI][newJ]->setF(cur->getF() + 1);
                    board[newI][newJ]->setParent(*cur);
                }
                opened_list.push(board[newI][newJ]);
                if(newI != targetX || newJ != targetY) board[newI][newJ]->setType(true);
            }
        }
    }
    return res;
}
void dijkstra(std::vector<Button*>& buttons, std::vector<std::vector<Space*>>& board, sf::Text* time) {
    int c = board[0].size();
    if (startClicked) {
        if (startX != -1 && startY != -1 && targetX != -1 && targetY != -1) {
            tStart = tEnd = std::chrono::steady_clock::now();
            pq2 open_list;
            board[startX][startY]->setF(0);
            open_list.push(board[startX][startY]);
            std::unordered_set<Space*> visited;
            dijkstraAlgo(open_list, visited, buttons, board, time);
            Space* x = board[targetX][targetY]->getParent();
            if(x) drawPath(x);
            startClicked = false;
            initializeCoordinates();
        }
        else startClicked = false;
    }
    else render(buttons, board, time);
}
//Deleter
void deleteBoard(std::vector<std::vector<Space*>>& board) {
    for (auto i : board) {
        for (auto& j : i) delete j;
    }
}
void deleteFonts() {
    delete regularFont;
    delete titleFont;
}
void deleteItems() {
    delete window;
    delete background;
    delete boardFrame;
}

int main() {
    initWindowAndBackground();
    initFonts();

    //MainState Initialization
    MainState mainState(window, *titleFont, *regularFont);

    //AlgoState Data initialization
    sf::Clock clock;
    float dt;
    boardFrame = initFrame();
    std::vector<std::vector<Space*>> board = initBoard();
    auto algoButtons = initButtons();
    MyText* title = initTitle();
    sf::Text* time = initTime();

    //Work
    while (window->isOpen()) {
        //Update Events
        sf::Event e;
        while (window->pollEvent(e)) {
            if (e.type == sf::Event::Closed)
                window->close();
        }
        switch (renderID) {
        case 0: {
            renderID = updateMainState(dt, &mainState);
            time = initTime();
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
                board = initBoard();
            }
            dfs(algoButtons, board, time);

        }; break;
        case 2: {
            //bfs
            if (firstTimeRenderingAlgo) {
                firstTimeRenderingAlgo = false;
                deleteBoard(board);
                board = initBoard();
            }
            bfs(algoButtons, board, time);

        }; break;
        case 3: {
            //gbs
            if (firstTimeRenderingAlgo) {
                firstTimeRenderingAlgo = false;
                deleteBoard(board);
                board = initBoard();
            }
            gbs(algoButtons, board, time);

        }; break;
        case 5: {
            //Dijkstra
            if (firstTimeRenderingAlgo) {
                firstTimeRenderingAlgo = false;
                deleteBoard(board);
                board = initBoard();
            }
            dijkstra(algoButtons, board, time);

        }; break;
        default: break;
        }
        dt = clock.restart().asMilliseconds();
    }
    // Freeing the memory
    deleteItems();
    deleteBoard(board);
    deleteFonts();
}