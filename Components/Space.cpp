#include "Space.h"


Space::Space(int x, int y, int width){
    this->x = x;
    this->y = y;
    this->f = INT32_MAX;
    this->parent = nullptr;
    visited = false;
    this->width = width;
    isPressed = false;
    this->type = -1;
    rec = new sf::RectangleShape(sf::Vector2f(width,width));
    rec->setPosition(x,y);
    rec->setOutlineThickness(1u);
    rec->setOutlineColor(sf::Color(19,19,19));
}

Space::~Space(){
    delete rec;
}

int Space::getType() const{
    return this->type;
}
void Space::update(float& dt, sf::Vector2f& mousePosition, int& type){
    if(rec->getGlobalBounds().contains(mousePosition)){
        if(sf::Mouse::isButtonPressed(sf::Mouse::Left)){
            isPressed = true;
            if(type == 1){ //Wall
                rec->setFillColor(sf::Color::Black);
            }
            else if(type == 2){ // Start
                rec->setFillColor(sf::Color::Green);
            }
            else if(type == 3){ // target
                rec->setFillColor(sf::Color::Red);
            }
            this->type = type;
        }
        else if(sf::Mouse::isButtonPressed(sf::Mouse::Right)){
            this->type = -1;
            isPressed = false;
            rec->setFillColor(sf::Color::White);
        }
        else isPressed = false;
    }
}

void Space::render(sf::RenderTarget& target){
    target.draw(*rec);
}

sf::Vector2i Space::getCoordinates() const{
    return sf::Vector2i(x,y);
}
bool Space::isSpaceClicked() const{
    return isPressed;
}
void Space::setType(bool x){
    if(x){
        rec->setFillColor(sf::Color::Cyan);
        type = 1;
    }
    else{
        rec->setFillColor(sf::Color::White);
        type = -1;
    }
}
bool Space::isVisited() const {
    return  visited;
}
void Space::setVisited() {
    visited = true;
}

void Space::colorPath(){
    rec->setFillColor(sf::Color::Magenta);
}
bool Space::isWall() const{
    return type == 1;
}
void Space::setPath() {
    rec->setFillColor(sf::Color(233, 116, 81));
}

void Space::setParent(Space& parent) {
    this->parent = &parent;
}

Space* Space::getParent() const {
    return this->parent;
}

void Space::setF(int val) {
    this->f = val;
}
int Space::getF() const {
    return this->f;
}