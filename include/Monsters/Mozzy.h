#pragma once
#include "Monsters/Monster.h"

class Mozzy : public Monster
{
public:
    //static constexpr int COST = 2;
    static constexpr int BASE_HEALTH = 80;
    static constexpr int BASE_ATTACK = 15;
    static constexpr int BASE_RANGE = 3;
    static constexpr int BASE_COOLDOWN = 4;

    //virtual bool canFly() const override { return true; } // כברירת מחדל מפלצות הן קרקעיות

    Mozzy(PlayerSide side);
    //void attack(Monster& target) override;

    std::unique_ptr<AttackAnimation> createAttackAnimation(BoardEntity* target) const override;

    // Freeze targets an enemy Monster - the base Monster::isValidSpecialTarget
    // default (enemy + Monster) already expresses exactly that, so no
    // override is needed here.
    bool specialAbilityNeedsTarget() const override { return true; }
private:
    void onSpecialAbility(Board& board, BoardEntity* target) override;
};
