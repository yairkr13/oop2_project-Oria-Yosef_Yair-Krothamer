#pragma once
#include "Monsters/Monster.h"

class Mozzy : public Monster
{
public:
    static constexpr int COST = 2;
    static constexpr int BASE_HEALTH = 80;
    static constexpr int BASE_ATTACK = 15;
    static constexpr int BASE_RANGE = 3;

    Mozzy(PlayerSide side);
    //void attack(Monster& target) override;
};
