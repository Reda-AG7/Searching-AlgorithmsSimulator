#include "MyText.h"

MyText::MyText(float pos_x, float pos_y, const std::string& value, 
                sf::Font& font, unsigned charSize, sf::Color color)
{
    this->value = value;
    content.setString(value);
    content.setCharacterSize(charSize);
    content.setFont(font);
    content.setFillColor(color);
    sf::FloatRect textRect = content.getGlobalBounds();
    content.setOrigin(sf::Vector2f(textRect.left + textRect.width/2.0f, textRect.top + textRect.height/2.0f));
    content.setPosition(pos_x,pos_y);
}

MyText::~MyText(){}

void MyText::update(float& dt){

}
void MyText::render(sf::RenderTarget& target){
    target.draw(content);
}