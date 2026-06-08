#pragma once
#include "SFML/Graphics.hpp"

class StaticObject
{
public:
	StaticObject(const sf::Vector2f& position, const sf::Vector2f& size);
	void draw(sf::RenderWindow& window) const;
private:
	//sf::RectangleShape m_shape;
};