#pragma once
#include "Monsters/Monster.h"

class Mozzy : public Monster
{
public:
    //static constexpr int COST = 2;
    static constexpr int BASE_HEALTH = 80;
    static constexpr int BASE_ATTACK = 15;
    static constexpr int BASE_RANGE = 3;

    //virtual bool canFly() const override { return true; } // כברירת מחדל מפלצות הן קרקעיות

    Mozzy(PlayerSide side);
    //void attack(Monster& target) override;
};
