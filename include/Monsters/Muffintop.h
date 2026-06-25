#pragma once
#include "Monsters/Monster.h"

class Muffintop : public Monster
{
public:
    static constexpr int COST = 3;
    static constexpr int BASE_HEALTH = 120;
    static constexpr int BASE_ATTACK = 25;
    static constexpr int BASE_RANGE = 2;

    Muffintop();
    //void attack(Monster& target) override;
};
