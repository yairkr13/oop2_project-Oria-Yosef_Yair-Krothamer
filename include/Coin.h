#pragma once
#include "SFML/Graphics.hpp"

class Coin
{
public:
	Coin();
	void draw(sf::RenderWindow& window) const;
private:
	sf::Vector2f m_position;
	sf::CircleShape m_shape;
};
