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

    std::unique_ptr<AttackAnimation> createAttackAnimation(BoardEntity* target) const override;
    // בתוך class Mozzy (תחת public):
    virtual std::string getSpecialAbilityDescription() const override {
        return "Freeze: Freezes an enemy monster, preventing it from taking actions during its next turn.";
    }
    // Freeze targets an enemy Monster - the base Monster::isValidSpecialTarget
    // default (enemy + Monster) already expresses exactly that, so no
    // override is needed here.
    bool specialAbilityNeedsTarget() const override { return true; }

    // White - matches the base default too, but declared explicitly so a
    // reader never has to wonder whether that's deliberate or coincidental.
    sf::Color getSpecialTargetHighlightColor() const override { return sf::Color(255, 255, 255, 180); }
private:
    void onSpecialAbility(Board& board, BoardEntity* target) override;
};
