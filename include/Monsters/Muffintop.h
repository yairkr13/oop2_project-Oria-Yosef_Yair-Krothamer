#pragma once
#include "Monsters/Monster.h"

// A monster whose Special (Heal) restores HP to a targeted ally.
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
    virtual std::string getSpecialAbilityDescription() const override;
    bool isValidSpecialTarget(const BoardEntity& candidate) const override;

    float scoreAsSpecialTarget(const BoardEntity& candidate) const override;

    sf::Color getSpecialTargetHighlightColor() const override;
private:
    void onSpecialAbility(const Board& board, BoardEntity* target) override;
};
