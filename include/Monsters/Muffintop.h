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

    std::unique_ptr<AttackAnimation> createAttackAnimation(sf::Vector2f targetPosition) const override;

    bool specialAbilityNeedsTarget() const override;
    // בתוך class Muffintop (תחת public):
    virtual std::string getSpecialAbilityDescription() const override;
    // Heal Ally targets a friendly Monster - flips the base (enemy)
    // default, but keeps its isAlive() requirement (see Monster's own
    // isValidSpecialTarget): a dying ally is still Tile-linked and must not
    // be targetable (e.g. healed) while its death animation plays.
    bool isValidSpecialTarget(const BoardEntity& candidate) const override;

    // AI preference: whoever is missing the most HP benefits most from a
    // heal - a full-health ally scores 0 (never preferred over someone
    // actually hurt).
    float scoreAsSpecialTarget(const BoardEntity& candidate) const override;

    sf::Color getSpecialTargetHighlightColor() const override;
private:
    void onSpecialAbility(const Board& board, BoardEntity* target) override;
};
