#pragma once
#include "Monsters/Monster.h"

class Henrietta : public Monster
{
public:
    static constexpr int COST = 3;
    static constexpr int BASE_HEALTH = 100;
    static constexpr int BASE_ATTACK = 20;
    static constexpr int BASE_RANGE = 3;

    Henrietta(PlayerSide side);
    //void attack(Monster& target) override;
};
