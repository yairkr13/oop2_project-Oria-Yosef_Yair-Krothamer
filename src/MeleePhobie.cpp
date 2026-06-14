#include "MeleePhobie.h"

MeleePhobie::MeleePhobie(const std::string& name, int attackPower, int range, int cost)
	: Monster(name, cost*150, cost*20, 1, cost)
{
}

void MeleePhobie::attack(DynamicObject& target)
{
	// Implement melee attack logic here
}
