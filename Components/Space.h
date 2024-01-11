#ifndef __SPACE__H
#define __SPACE__H
#include "SFML/Graphics.hpp"

class Space{

    private:
        sf::RectangleShape* rec;
        int x, y;
        int width;
        int type;
        bool isPressed;
        bool visited;
    public:
    Space(int, int, int);
    ~Space();
    void update(float& dt, sf::Vector2f&, int&);
    void render(sf::RenderTarget&);
    bool isSpaceClicked() const;
    sf::Vector2f getCoordinates() const;
    int getType() const;
    void setType(bool);
    bool isVisited() const;
    void setVisited();
    void colorPath();
    bool isWall() const;
};

#endif