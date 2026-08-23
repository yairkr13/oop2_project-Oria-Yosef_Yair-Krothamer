#pragma once
#include "Monsters/Monster.h"

class Blue : public Monster
{
public:
    //static constexpr int COST = 3;
    static constexpr int BASE_HEALTH = 120;
    static constexpr int BASE_ATTACK = 25;
    static constexpr int BASE_RANGE = 1;
    static constexpr int BASE_COOLDOWN = 3;

    Blue(PlayerSide side);

    std::unique_ptr<AttackAnimation> createAttackAnimation(BoardEntity* target) const override;

    // Knockback targets an enemy Monster - the base Monster::isValidSpecialTarget
    // default (enemy + Monster) already expresses exactly that.
    bool specialAbilityNeedsTarget() const override { return true; }
private:
    void onSpecialAbility(Board& board, BoardEntity* target) override;
};
