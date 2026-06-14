#pragma once
#include <SFML/Graphics.hpp>
#include "StaticObject.h"

class Player : public StaticObject
{
public:

    void draw(sf::RenderWindow& window) const;
    const sf::Vector2f& getPosition() const;

private:
    sf::Sprite m_sprite;
    int m_health;
    int m_keys;
};
