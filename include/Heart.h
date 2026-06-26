#pragma once
#include "StaticObject.h"
#include "Constants.h"
#include "Tile.h"

class Heart : public Tile
{
public:
    Heart(PlayerSide side, const sf::Vector2f& position);

    void draw(sf::RenderWindow& window) const;
    void takeDamage(int damage);
    bool isAlive() const { return m_health > 0; }
    int getHealth() const { return m_health; }
    PlayerSide getSide() const { return m_side; }

private:
    int m_health = 10;
    int m_maxHealth = 10;
    PlayerSide m_side;
    sf::Sprite m_sprite;

    void drawHealthBar(sf::RenderWindow& window) const;
};