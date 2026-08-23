#pragma once
#include "Monsters/Monster.h"

class Henrietta : public Monster
{
public:
    //static constexpr int COST = 3;
    static constexpr int BASE_HEALTH = 100;
    static constexpr int BASE_ATTACK = 20;
    static constexpr int BASE_RANGE = 3;
    static constexpr int BASE_COOLDOWN = 5;

    Henrietta(PlayerSide side);

    std::unique_ptr<AttackAnimation> createAttackAnimation(BoardEntity* target) const override;

    bool specialAbilityNeedsTarget() const override { return true; }

    // Protection targets a friendly Monster - flips the base (enemy) default.
    bool isValidSpecialTarget(BoardEntity& candidate) const override
    {
        return candidate.asMonster() != nullptr && candidate.isAllyOf(getSide());
    }

    sf::Color getSpecialTargetHighlightColor() const override { return sf::Color(255, 165, 0, 180); } // orange
private:
    void onSpecialAbility(Board& board, BoardEntity* target) override;
    //void attack(Monster& target) override;
};
