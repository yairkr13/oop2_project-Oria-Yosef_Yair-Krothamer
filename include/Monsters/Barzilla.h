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

    // Overridden (not the base Monster::attack) so the next successful
    // attack after Empowered Attack is armed can apply the ×2 multiplier
    // and commit it - see m_empoweredAttack below.
    void attack(BoardEntity* target) override;

    std::unique_ptr<AttackAnimation> createAttackAnimation(BoardEntity* target) const override;

    // Self/no-target Special - the base Monster::specialAbilityNeedsTarget()
    // default (false) already fits, no override needed.

    // Selecting the Card only arms the bonus; it does not itself use an
    // action or reset the cooldown. Barzilla::attack() commits both, only
    // once the empowered attack actually resolves.
    bool specialAbilityCommitsOnSelect() const override { return false; }

    // Un-arms an activated-but-not-yet-attacked bonus (Card clicked again,
    // or the player switched to a different pending Card) without touching
    // action/cooldown - neither was ever consumed by arming in the first
    // place.
    void cancelSpecialAbility() override { m_empoweredAttack = false; }
private:
    void onSpecialAbility(Board& board, BoardEntity* target) override;

    // Safety net for the case GameplayState can't directly observe: the
    // turn ends (by any path, not just the Space key) with the bonus still
    // armed but unused. Expires it rather than letting it carry into a
    // future turn.
    void onTurnBoundary() override;

    bool m_empoweredAttack = false;
};
