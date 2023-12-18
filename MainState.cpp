#include "MainState.h"

MainState::MainState(sf::RenderWindow* window, sf::Font& titleFont,sf::Font& regularFont){
    //init background
    background = new sf::RectangleShape(sf::Vector2f(window->getSize().x,window->getSize().y));
    background->setFillColor(sf::Color(28, 40, 65));
    //init Title
    title = new MyText(window->getSize().x/2,60,"Searching Simulator",titleFont,40u,sf::Color::White);
    //init logo
    texture = new sf::Texture();
    texture->loadFromFile("Images/logo.png");
    sprite = new sf::Sprite(*texture);
    sf::FloatRect logoRec = sprite->getGlobalBounds();
    sprite->setOrigin(logoRec.left + logoRec.width/2.0f, logoRec.top + logoRec.height/2.0f);
    sprite->setPosition(window->getSize().x/2.0f, 300.0f);
    sprite->setScale(300.0f/(float)texture->getSize().x,300.0f/(float)texture->getSize().y);
    
    //init Buttons
    buttons.resize(4);
    float start = 500.0f;
    buttons[0] = new Button(window->getSize().x/2.0f,start, 300.0f,50.0f,"Depth First Search (DFS)",regularFont,25u,sf::Color::White);
    buttons[1] = new Button(window->getSize().x/2.0f,start+65.0f, 300.0f,50.0f,"Breath First Search (BFS)",regularFont,25u,sf::Color::White);
    buttons[2] = new Button(window->getSize().x/2.0f,start+130.0f, 300.0f,50.0f,"Greedy Best First (GBS)",regularFont,25u,sf::Color::White);
    buttons[3] = new Button(window->getSize().x/2.0f,start+195.0f, 180.0f,50.0f,"Exit",regularFont,25u,sf::Color::White);
}

MainState::~MainState(){
    delete background;
    delete title;
    for(auto& i : buttons)
        delete i;
}
int MainState::update(float& dt, sf::Vector2f& mousePosition){
    //Only button can get updated here
    //so ...
    int res = 0;
    for(int i=0;i<buttons.size();i++){
        buttons[i]->update(dt,mousePosition);
        if(buttons[i]->isButtonClicked()){
            res = i+1;
            return res;
        }
    }
    return res;
}

void MainState::render(sf::RenderTarget& target){
    target.draw(*background);
    title->render(target);
    for(auto& i : buttons)
        i->render(target);
    target.draw(*sprite);

}