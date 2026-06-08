#pragma once
#include <SFML/Graphics.hpp>

class Player
{
public:
    Player();

    void update(float dt);
    void draw(sf::RenderWindow& window) const;

private:
    sf::RectangleShape m_shape;
    float m_angleDeg = 0.f;

    static constexpr float SPEED = 180.f;
    static constexpr float ROTATION_SPEED = 120.f;
};
