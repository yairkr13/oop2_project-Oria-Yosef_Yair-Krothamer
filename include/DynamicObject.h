#pragma once
#include "SFML/Graphics.hpp"

class DynamicObject
{
public:
	DynamicObject(const sf::Vector2f& position, const sf::Vector2f& size);
	void update();
};