#include "Button.h"

Button::Button(float pos_x, float pos_y, float width, float height, const std::string& value, sf::Font& font, unsigned charSize, sf::Color color)
{
    rect.setSize(sf::Vector2f(width, height));
    rect.setOutlineColor(sf::Color::White);
    rect.setOutlineThickness(2u);
    rect.setFillColor(sf::Color::Transparent);
    sf::FloatRect rectRect = rect.getGlobalBounds();
    rect.setOrigin(sf::Vector2f(rectRect.left + rectRect.width/2.0f,rectRect.top + rectRect.height/2.0f));
    rect.setPosition(sf::Vector2f(pos_x,pos_y));

    content = new MyText(pos_x, pos_y, value, font, charSize, color);

    idle = sf::Color(28, 40, 65);
    hover = sf::Color(46, 80, 144);
    active = sf::Color(0, 204, 255);
    
}

Button::~Button(){
    delete content;
}

void Button::update(float& dt, const sf::Vector2f& mousePosition){
    if(rect.getGlobalBounds().contains(mousePosition)){
        //hover
        rect.setFillColor(hover);
        if(sf::Mouse::isButtonPressed(sf::Mouse::Left)){
            isPressed = true;
            //Active
            rect.setFillColor(active);
        }
    }
    else {
        rect.setFillColor(idle);
        isPressed = false;
    }

}

void Button::render(sf::RenderTarget& target){
    target.draw(rect);
    content->render(target);
}

bool Button::isButtonClicked() const
{
    return isPressed;
}
