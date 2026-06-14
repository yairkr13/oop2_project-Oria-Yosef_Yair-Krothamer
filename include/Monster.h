#pragma once
#include "DynamicObject.h"

class Monster : public DynamicObject
{
public:
	Monster(const std::string& name, int health, int attackPower, int range, int cost);
	virtual void attack(DynamicObject& target) = 0;
	void takeDamage(int damage);
	//virtual void move(Tile* targetTile);
	bool isAlive() const;
private:
	int m_health;
	int m_attackDamage;
	int m_range; //movement range and attack range
	int m_cost;
	//MovementType m_movementType;
	//belong to faction????????????????????????????
};