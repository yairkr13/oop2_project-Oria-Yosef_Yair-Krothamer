#pragma once
#include <SFML/Graphics.hpp>
#include "StaticObject.h"
#include "Heart.h"

class Player //: public StaticObject do an heart tile
{
public:

    //void draw(sf::RenderWindow& window) const;
    //const sf::Vector2f& getPosition() const;

private:
    //sf::Sprite m_sprite;
    //int m_health;
	Heart m_heart;
    int m_keys;
    //std::vector<std::unique_ptr<Monster>> m_monsters;
};
