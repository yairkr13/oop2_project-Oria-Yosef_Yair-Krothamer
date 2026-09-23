#pragma once
#include "Monsters/Monster.h"

// A monster whose Special (Protection) shields a targeted ally from all
// damage until the next turn.
class Henrietta : public Monster
{
public:
    static constexpr int BASE_HEALTH = 100;
    static constexpr int BASE_ATTACK = 20;
    static constexpr int BASE_RANGE = 3;
    static constexpr int BASE_COOLDOWN = 5;

    Henrietta(PlayerSide side);

    std::unique_ptr<AttackAnimation> createAttackAnimation(sf::Vector2f targetPosition) const override;

    bool specialAbilityNeedsTarget() const override;

    bool isValidSpecialTarget(const BoardEntity& candidate) const override;

    float scoreAsSpecialTarget(const BoardEntity& candidate) const override;
    virtual std::string getSpecialAbilityDescription() const override;
    sf::Color getSpecialTargetHighlightColor() const override;
private:
    void onSpecialAbility(const Board& board, BoardEntity* target) override;
};
