#pragma once
#include "StaticObject.h"

class Tile : public StaticObject
{
public:
	Tile(const sf::Texture& texture, const sf::Vector2f& position);
	void draw(sf::RenderWindow& window) const override;
private:
	int m_row;
	int m_q;
	bool m_isPassable;
};