#pragma once
#include "Monsters/Monster.h"

class Blue : public Monster
{
public:
    //static constexpr int COST = 3;
    static constexpr int BASE_HEALTH = 120;
    static constexpr int BASE_ATTACK = 25;
    static constexpr int BASE_RANGE = 1;

    Blue(PlayerSide side);
    //void attack(Monster& target) override;
private:
    void onSpecialAbility(BoardEntity* target) override;
};
