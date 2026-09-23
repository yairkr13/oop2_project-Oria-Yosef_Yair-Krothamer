#pragma once
#include "Monsters/Monster.h"

// A flying monster whose Special (Knockback) pushes an enemy back along
// the attacker-to-target direction.
class Blue : public Monster
{
public:
    //static constexpr int COST = 3;
    static constexpr int BASE_HEALTH = 120;
    static constexpr int BASE_ATTACK = 25;
    static constexpr int BASE_RANGE = 1;
    static constexpr int BASE_COOLDOWN = 3;

    Blue(PlayerSide side);

    std::unique_ptr<AttackAnimation> createAttackAnimation(sf::Vector2f targetPosition) const override;

    bool specialAbilityNeedsTarget() const override;
    virtual std::string getSpecialAbilityDescription() const override;
    sf::Color getSpecialTargetHighlightColor() const override;
private:
    void onSpecialAbility(const Board& board, BoardEntity* target) override;
};
