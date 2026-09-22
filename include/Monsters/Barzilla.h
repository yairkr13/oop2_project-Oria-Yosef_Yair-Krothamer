#pragma once
#include "Monsters/Monster.h"

class Barzilla : public Monster
{
public:
    //static constexpr int COST = 4;
    static constexpr int BASE_HEALTH = 150;
    static constexpr int BASE_ATTACK = 30;
    static constexpr int BASE_RANGE = 2;
    static constexpr int BASE_COOLDOWN = 3;

    Barzilla(PlayerSide side);

    // Empowered Attack is now ally-targeted (see onSpecialAbility) - Barzilla
    // no longer buffs its own next attack, so it no longer needs to override
    // attack()/getAttackRange(), or defer-and-cancel the Special the way the
    // old self-buff version did. Old code kept as comments immediately below
    // each removed member, not deleted, per request.
    //
    // void attack(BoardEntity* target) override;
    // int getAttackRange() const override { return m_empoweredAttack ? m_range * 2 : m_range; }
    // bool specialAbilityCommitsOnSelect() const override { return false; }
    // void cancelSpecialAbility() override { m_empoweredAttack = false; }

    std::unique_ptr<AttackAnimation> createAttackAnimation(sf::Vector2f targetPosition) const override;

    // Empowered Attack now targets an ally (mirrors Henrietta's Protection -
    // the buff lives on the recipient, granted immediately on selection, not
    // armed on Barzilla for later).
    bool specialAbilityNeedsTarget() const override { return true; }

    // Ally-targeted, like Muffintop's Heal/Henrietta's Protection - flips
    // the base (enemy) default. Barzilla can target himself too (same side),
    // same as those two Specials already allow.
    bool isValidSpecialTarget(const BoardEntity& candidate) const override
    {
        return candidate.isAlive() && candidate.canBeTargetedBySpecial() && candidate.isAllyOf(getSide());
    }

    //
    virtual std::string getSpecialAbilityDescription() const override {
        return "Empowered Attack: Doubles the damage of a chosen ally's next attack.";
    }

    // Distinct from every other monster's color (and from Mozzy, which
    // shares the base Monster default white) - same purple family the
    // project's old extended-attack-range highlight used to use.
    sf::Color getSpecialTargetHighlightColor() const override { return sf::Color(190, 90, 230, 180); } // purple
private:
    void onSpecialAbility(Board& board, BoardEntity* target) override;

    // No longer needed: the armed bonus used to live on Barzilla himself and
    // had to expire if never spent (see BoardEntity::onTurnBoundary's own
    // generic per-turn tick, still used by Protection). Now it lives on
    // whichever ally received it (m_empoweredAttack on Monster), the same
    // place Protection's flag lives - and unlike Protection it has no
    // turn-count expiry of its own, it simply waits for that ally's next
    // attack to consume it.
    // void onTurnBoundary() override;
};
