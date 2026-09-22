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

    std::unique_ptr<AttackAnimation> createAttackAnimation(sf::Vector2f targetPosition) const override;

    // Knockback targets an enemy Monster - the base Monster::isValidSpecialTarget
    // default (enemy + Monster) already expresses exactly that.
    bool specialAbilityNeedsTarget() const override { return true; }
    // בתוך class Blue (תחת public):
    virtual std::string getSpecialAbilityDescription() const override {
        return "Knockback: Knocks back a targeted enemy monster on the grid.";
    }
    sf::Color getSpecialTargetHighlightColor() const override { return sf::Color(60, 120, 255, 180); } // blue
private:
    void onSpecialAbility(const Board& board, BoardEntity* target) override;
};
