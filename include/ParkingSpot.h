#pragma once
#include "SFML/Graphics.hpp"
#include "StaticObject.h"

class ParkingSpot :public StaticObject
{
public:
	ParkingSpot(const sf::Vector2f& position, const sf::Vector2f& size);
	void draw(sf::RenderWindow& window) const;
private:
	sf::RectangleShape m_shape;
};