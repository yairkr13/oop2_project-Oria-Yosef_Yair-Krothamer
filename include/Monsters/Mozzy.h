#pragma once
#include "Monsters/Monster.h"

// A flying monster whose Special (Freeze) prevents an enemy from acting
// on its next turn.
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

    std::unique_ptr<AttackAnimation> createAttackAnimation(sf::Vector2f targetPosition) const override;
    virtual std::string getSpecialAbilityDescription() const override;
    bool specialAbilityNeedsTarget() const override;

    float scoreAsSpecialTarget(const BoardEntity& candidate) const override;

    sf::Color getSpecialTargetHighlightColor() const override;
private:
    void onSpecialAbility(const Board& board, BoardEntity* target) override;
};
