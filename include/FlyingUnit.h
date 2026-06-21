#pragma once
#include "Monsters/Monster.h"

class FlyingUnit : public Monster
{
public:
	FlyingUnit(const std::string& name, int health, int attackPower, int range, int cost);
	//void move(Tile* targetTile) override;

};