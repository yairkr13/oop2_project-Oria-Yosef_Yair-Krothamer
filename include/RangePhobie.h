#pragma once 
#include "Monster.h"

class RangePhobie : public Monster
{
	public:
	RangePhobie(const std::string& name, int attackPower, int range, int cost);
	void attack(DynamicObject& target) override;
	//void move(Tile* targetTile) override;
};