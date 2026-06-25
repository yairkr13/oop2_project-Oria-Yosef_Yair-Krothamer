#pragma once
#include "Monsters/Monster.h"

class Barzilla : public Monster
{
public:
    static constexpr int COST = 4;
    static constexpr int BASE_HEALTH = 150;
    static constexpr int BASE_ATTACK = 30;
    static constexpr int BASE_RANGE = 2;

    Barzilla();
    //void attack(Monster& target) override;
};
