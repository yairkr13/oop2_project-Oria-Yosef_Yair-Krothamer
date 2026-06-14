#pragma once
#include "SFML/Graphics.hpp"
#include "GameObject.h"

class DynamicObject :public GameObject
{
public:
	DynamicObject(const sf::Vector2f& position, const sf::Vector2f& size);
	virtual void update() = 0;
};