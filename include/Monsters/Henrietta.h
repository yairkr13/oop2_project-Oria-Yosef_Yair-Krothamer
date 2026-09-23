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

    std::unique_ptr<AttackAnimation> createAttackAnimation(sf::Vector2f targetPosition) const override;

    bool specialAbilityNeedsTarget() const override;

    // Protection targets a friendly Monster - flips the base (enemy)
    // default, but keeps its isAlive() requirement (see Monster's own
    // isValidSpecialTarget): a dying ally is still Tile-linked and must not
    // be targetable while its death animation plays. Also keeps the base's
    // canBeTargetedBySpecial() check - Heart doesn't override that (stays
    // false), so Protection can't target it either, same as every other
    // Special.
    bool isValidSpecialTarget(const BoardEntity& candidate) const override;

    // AI preference: whoever has the least HP is the most vulnerable, so
    // the most worth shielding - same formula as Muffintop's Heal, just
    // repurposed for "who needs protecting" instead of "who needs healing."
    float scoreAsSpecialTarget(const BoardEntity& candidate) const override;
    // בתוך class Henrietta (תחת public):
    virtual std::string getSpecialAbilityDescription() const override;
    sf::Color getSpecialTargetHighlightColor() const override;
private:
    void onSpecialAbility(const Board& board, BoardEntity* target) override;
    //void attack(Monster& target) override;
};
