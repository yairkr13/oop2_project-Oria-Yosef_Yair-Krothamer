#pragma once
#include <SFML/Graphics.hpp>

class Player
{
public:
    Player(const sf::Texture& texture);
    Player()=default;

    void update(float dt);
    void draw(sf::RenderWindow& window) const;
    const sf::Vector2f& getPosition() const;
private:
    //sf::RectangleShape m_shape;
    sf::Sprite m_sprite;
    float m_angleDeg = 0.f;

    static constexpr float SPEED = 180.f;
    static constexpr float ROTATION_SPEED = 120.f;
};
