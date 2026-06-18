#pragma once
#include "SFML/Graphics.hpp"
//#include "StaticObject.h"

class Tile //: public StaticObject
{
public:
	Tile(int q, int row, const sf::Vector2f& position);
	void draw(sf::RenderWindow& window) const;
	int getQ() const;
	int getRow() const;
	bool isPassable() const { return m_isPassable; }
	void setHighlighted(bool highlighted);
private:
	int m_row;
	int m_q;
	bool m_isPassable;
	sf::CircleShape m_shape;
};