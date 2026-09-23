#pragma once
#include "Monsters/Monster.h"

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
    // בתוך class Mozzy (תחת public):
    virtual std::string getSpecialAbilityDescription() const override;
    // Freeze targets an enemy Monster - the base Monster::isValidSpecialTarget
    // default (enemy + Monster) already expresses exactly that, so no
    // override is needed here.
    bool specialAbilityNeedsTarget() const override;

    // AI preference: the enemy with the MOST HP is the biggest ongoing
    // threat (takes longest to kill), so the most worth freezing out of a
    // turn - the opposite direction from Heal/Protection's "least HP" above.
    float scoreAsSpecialTarget(const BoardEntity& candidate) const override;

    // White - matches the base default too, but declared explicitly so a
    // reader never has to wonder whether that's deliberate or coincidental.
    sf::Color getSpecialTargetHighlightColor() const override;
private:
    void onSpecialAbility(const Board& board, BoardEntity* target) override;
};
