#ifndef _MAIN_STATE__H
#define _MAIN_STATE__H
#include "SFML/Graphics.hpp"
#include "Components/Button.h"

class MainState{
    private:
        sf::RectangleShape* background;
        std::vector<Button*> buttons;
        MyText* title;
        sf::Texture* texture;
        sf::Sprite* sprite;
    public:
        MainState(sf::RenderWindow*,sf::Font& titleFont,sf::Font& regularFont);
        ~MainState();
        int update(float&, sf::Vector2f& );
        void render(sf::RenderTarget&);
};

#endif