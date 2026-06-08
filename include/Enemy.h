#pragma once
#include "SFML/Graphics.hpp"

class Enemy
{
public:
	Enemy();
	void update();
	void draw(sf::RenderWindow& window) const;
private:
	sf::Vector2f m_size;
	double m_speed;
	double m_direction; // Angle in radians
};