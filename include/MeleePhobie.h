#pragma once
#include "Monster.h"

class MeleePhobie : public Monster
{
public:
	MeleePhobie(const std::string& name, int health, int attackPower, int range, int cost);
	void attack(DynamicObject& target) override;
	//void move(Tile* targetTile) override;
};