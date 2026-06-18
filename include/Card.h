#pragma once
#include "SFML/Graphics.hpp"
#include <string>
#include "StaticObject.h"
#include "Monster.h"

class Card:public StaticObject
{
public:
    Card(const sf::Texture& texture, const sf::Vector2f& position);
    void draw(sf::RenderWindow& window) const override;

private:
    //sf::Sprite m_sprite;
    int m_cost;
    std::unique_ptr<Monster> m_monster;
    std::string m_monsterName;
};
