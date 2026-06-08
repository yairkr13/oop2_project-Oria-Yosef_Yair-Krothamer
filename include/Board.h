#pragma once
#include "SFML/Graphics.hpp"

class Board
{
public:
	Board();
	void draw(sf::RenderWindow& window) const;
};