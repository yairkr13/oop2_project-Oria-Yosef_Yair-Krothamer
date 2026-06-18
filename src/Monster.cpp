#include "Monster.h"

Monster::Monster(const std::string& name, int health, int attackPower, int range, int cost)
	: GameObject(sf::Texture(), sf::Vector2f()), m_health(health), m_attackDamage(attackPower), m_range(range), m_cost(cost)
{
}

void Monster::takeDamage(int damage)
{
	m_health -= damage;
	if (m_health < 0) m_health = 0;
}

int Monster::getRange() const
{
	return m_range;
}

//void Monster::handleClick(const sf::Vector2f& pos)
//{
//
//}