#pragma once
#include "SFML/Graphics.hpp"
#include "GameObject.h"

class StaticObject : public GameObject
{
public:
	StaticObject(const sf::Vector2f& position, const sf::Vector2f& size);
private:
	//sf::RectangleShape m_shape;
};