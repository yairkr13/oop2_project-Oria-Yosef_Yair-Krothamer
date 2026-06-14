#pragma once
#include "StaticObject.h"

class Heart : public StaticObject
{
public:
	Heart(const sf::Texture& texture, const sf::Vector2f& position);
	void draw(sf::RenderWindow& window) const override;
private:
	int m_health;
};
