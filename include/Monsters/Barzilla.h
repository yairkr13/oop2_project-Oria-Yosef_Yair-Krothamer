#pragma once
#include "Monsters/Monster.h"

// A monster whose Special (Empowered Attack) doubles an ally's next
// attack damage, rather than buffing itself.
class Barzilla : public Monster
{
public:
    static constexpr int BASE_HEALTH = 150;
    static constexpr int BASE_ATTACK = 30;
    static constexpr int BASE_RANGE = 2;
    static constexpr int BASE_COOLDOWN = 3;

    Barzilla(PlayerSide side);

    // Empowered Attack is ally-targeted, so Barzilla no longer buffs its own attack.
    std::unique_ptr<AttackAnimation> createAttackAnimation(sf::Vector2f targetPosition) const override;

    bool specialAbilityNeedsTarget() const override;

    bool isValidSpecialTarget(const BoardEntity& candidate) const override;

    virtual std::string getSpecialAbilityDescription() const override;

    sf::Color getSpecialTargetHighlightColor() const override;
private:
    void onSpecialAbility(const Board& board, BoardEntity* target) override;

};
