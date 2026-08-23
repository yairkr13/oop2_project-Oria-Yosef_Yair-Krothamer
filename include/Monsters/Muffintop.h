#pragma once
#include "Monsters/Monster.h"

class Muffintop : public Monster
{
public:
    //static constexpr int COST = 3;
    static constexpr int BASE_HEALTH = 120;
    static constexpr int BASE_ATTACK = 25;
    static constexpr int BASE_RANGE = 2;
    static constexpr int BASE_COOLDOWN = 3;

    Muffintop(PlayerSide side);
	//bool useSpecialAbility(BoardEntity* target) override;

    std::unique_ptr<AttackAnimation> createAttackAnimation(BoardEntity* target) const override;

    bool specialAbilityNeedsTarget() const override { return true; }

    // Heal Ally targets a friendly Monster - flips the base (enemy) default.
    bool isValidSpecialTarget(BoardEntity& candidate) const override
    {
        return candidate.asMonster() != nullptr && candidate.isAllyOf(getSide());
    }
private:
    void onSpecialAbility(Board& board, BoardEntity* target) override;
};
