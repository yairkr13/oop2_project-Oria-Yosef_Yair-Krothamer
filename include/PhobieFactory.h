#pragma once
#include "Monsters/Monster.h"

class PhobieFactory
{
public:
	static std::unique_ptr<Monster> createMeleePhobie(const std::string& name, int attackPower, int range, int cost)
	{
		return std::make_unique<MeleePhobie>(name, attackPower, range, cost);
	}
	instance(const std::string & monster)
	{
		return m_creators.at(it->first)(monster, 10, 1, 1);
	}
private:
	std::map<std::string, std::function<std::unique_ptr<Monster>(const std::string&, int, int, int)>> m_creators;
};
