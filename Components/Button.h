#ifndef __BUTTON__H
#define __BUTTON__H
#include "MyText.h"
class Button{
    private:
        sf::RectangleShape rect;
        MyText* content;
        bool isPressed;
        sf::Color idle, hover, active;
    public:
        Button(float, float, float, float, const std::string&, sf::Font&, unsigned, sf::Color);
        ~Button();
        void update(float&, const sf::Vector2f&);
        void render(sf::RenderTarget&);
        bool isButtonClicked() const;
};


#endif