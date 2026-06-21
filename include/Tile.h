#pragma once
#include "SFML/Graphics.hpp"
#include "Monster.h"
#include <memory>
//#include "StaticObject.h"

class Tile //: public StaticObject
{
public:
	Tile(int q, int row, const sf::Vector2f& position);
	void draw(sf::RenderWindow& window) const;
	int getQ() const { return m_q; }
	int getRow() const { return m_row; }
	bool isPassable() const { return m_isPassable; }
	bool isHighlighted() const { return m_isHighlighted; }
	void setHighlighted(bool highlighted);

	void setMonster(std::shared_ptr<Monster> monster) { m_monsterRef = monster; };
	std::shared_ptr<Monster> getMonster() const { return m_monsterRef.lock(); }
	bool hasMonster() const { return !m_monsterRef.expired(); }
private:
	int m_row;
	int m_q;
	bool m_isPassable;
	bool m_isHighlighted = false;
	sf::CircleShape m_shape;
	std::weak_ptr<Monster> m_monsterRef;
};