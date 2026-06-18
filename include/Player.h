#pragma once
#include <SFML/Graphics.hpp>
#include "StaticObject.h"
#include "Heart.h"
#include "Card.h"
#include <vector>   
#include <memory>
//check
class Player //: public StaticObject do an heart tile
{
public:
    Player() {};
	//bool handleClick(const sf::Vector2f& pos);
    Card* handleCardClick(sf::Vector2f mousePos);
    //void draw(sf::RenderWindow& window) const;
    //const sf::Vector2f& getPosition() const;

private:
    //sf::Sprite m_sprite;
    //int m_health;
	std::unique_ptr<Heart> m_heart;
    //Heart* m_heart;
	std::vector<std::unique_ptr<Card>> m_cards;
    int m_keys;
    //std::vector<std::unique_ptr<Monster>> m_monsters;
};
