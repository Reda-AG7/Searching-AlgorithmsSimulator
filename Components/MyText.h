#ifndef __MY_TEXT__H
#define __MY_TEXT__H
#include <string>
#include "SFML/Graphics.hpp"


class MyText{
    private:
        sf::Text content;
        std::string value;
        float pos_x,pos_y;
    public:
        MyText(float, float, const std::string&, sf::Font&, unsigned,sf::Color);
        ~MyText();
        void update(float&);
        void render(sf::RenderTarget&);

};


#endif