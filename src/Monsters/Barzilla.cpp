#include "Monsters/Barzilla.h"
#include "Attacks/SplashAttackAnimation.h"
#include "Attacks/BurstProjectileAnimation.h"
#include "Attacks/GrowingEffectAnimation.h"
#include "AssetsManager.h"
#include "Constants.h"

namespace
{
    constexpr float FIRE_BLAST_DURATION = 0.5f; // seconds for the blast to travel from Barzilla to its target

    // Proportional to Config::MONSTER_BOARD_SIZE, like Mozzy's ACID_SPLASH_THICKNESS.
    constexpr float FIRE_BLAST_THICKNESS = Config::MONSTER_BOARD_SIZE * 0.5f;

    // Old empowered-projectile constants, kept as a comment - Barzilla's
    // own attack no longer varies visually by m_empoweredAttack:
    //
    // constexpr int EMPOWERED_FIRE_BLAST_COUNT = 3;
    // constexpr float EMPOWERED_FIRE_BLAST_LAUNCH_INTERVAL = 0.04f;
    // constexpr float EMPOWERED_FIRE_BLAST_TRAVEL_DURATION = 0.4f;
    // constexpr float EMPOWERED_FIRE_BLAST_SIZE = Config::MONSTER_BOARD_SIZE * 0.55f;

    // Grant visual, played on the ally the instant Barzilla empowers them -
    // a quick grow-and-fade glow.
    constexpr float EMPOWERED_IMPACT_SIZE = Config::MONSTER_BOARD_SIZE * 1.3f;
    constexpr float EMPOWERED_IMPACT_GROW_DURATION = 0.12f;
    constexpr float EMPOWERED_IMPACT_HOLD_DURATION = 0.08f;
    constexpr float EMPOWERED_IMPACT_FADE_DURATION = 0.15f;

    // Empowered Attack's damage multiplier (applied in Monster::attack()).
    constexpr float EMPOWERED_ATTACK_MULTIPLIER = 2.f;

    // Attack sheet timing: matches a single FireBlast's ~0.5s travel time.
    constexpr float ATTACK_FRAME_DURATION = 0.02f;
}

Barzilla::Barzilla(PlayerSide side)
    : Monster(side, BASE_HEALTH, BASE_ATTACK, BASE_RANGE, BASE_COOLDOWN, -1, -1, sf::Color::Red, "barzilla")
{
    setStandardSpriteAnimations("barzilla", "barzilla_walk", ATTACK_FRAME_DURATION);
}

// Old self-buff version, kept as a comment (not deleted) per request:
//
// void Barzilla::attack(BoardEntity* target)
// {
//     bool empowered = m_empoweredAttack;
//     int damage = empowered ? m_attackDamage * 2 : m_attackDamage;
//     if (empowered)
//     {
//         m_empoweredAttack = false;
//         m_specialCooldown = m_baseCooldown;
//         const sf::Texture& impactTexture = AssetsManager::getInstance().getTexture("fire_blast");
//         auto impactFlash = std::make_unique<GrowingEffectAnimation>(
//             impactTexture, target->getScreenPosition(), EMPOWERED_IMPACT_SIZE,
//             EMPOWERED_IMPACT_GROW_DURATION, EMPOWERED_IMPACT_HOLD_DURATION, EMPOWERED_IMPACT_FADE_DURATION);
//         target->playSpecialAbilityAnimation(std::move(impactFlash));
//     }
//     target->takeDamage(damage);
//     useAction();
// }

// Grants the target's next attack double damage (consumed in
// Monster::attack()); the grow effect plays on the target as the buff lands.
void Barzilla::onSpecialAbility(const Board& board, BoardEntity* target)
{
    if (!target) return;

    target->applyEmpoweredAttack(EMPOWERED_ATTACK_MULTIPLIER);

    const sf::Texture& impactTexture = AssetsManager::getInstance().getTexture("fire_blast");
    auto empowerEffect = std::make_unique<GrowingEffectAnimation>(
        impactTexture, target->getScreenPosition(), EMPOWERED_IMPACT_SIZE,
        EMPOWERED_IMPACT_GROW_DURATION, EMPOWERED_IMPACT_HOLD_DURATION, EMPOWERED_IMPACT_FADE_DURATION);
    target->playSpecialAbilityAnimation(std::move(empowerEffect));
}

// Old onTurnBoundary override, kept as a comment - no longer needed since
// the buff now lives on the ally, not on Barzilla:
//
// void Barzilla::onTurnBoundary()
// {
//     BoardEntity::onTurnBoundary();
//     m_empoweredAttack = false;
// }

std::unique_ptr<AttackAnimation> Barzilla::createAttackAnimation(sf::Vector2f targetPosition) const
{
    const sf::Texture& fireBlastTexture = AssetsManager::getInstance().getTexture("fire_blast");

    // Old empowered-burst branch, kept as a comment - Barzilla's own
    // attack no longer varies by m_empoweredAttack:
    //
    // if (m_empoweredAttack)
    // {
    //     return std::make_unique<BurstProjectileAnimation>(
    //         fireBlastTexture, m_screenPos, target->getScreenPosition(),
    //         EMPOWERED_FIRE_BLAST_COUNT, EMPOWERED_FIRE_BLAST_LAUNCH_INTERVAL,
    //         EMPOWERED_FIRE_BLAST_TRAVEL_DURATION, EMPOWERED_FIRE_BLAST_SIZE);
    // }

    // Grows/reveals from attacker to target, same mechanism as Mozzy's acid splash.
    return std::make_unique<SplashAttackAnimation>(
        fireBlastTexture, m_screenPos, targetPosition, FIRE_BLAST_DURATION, FIRE_BLAST_THICKNESS);
}

bool Barzilla::specialAbilityNeedsTarget() const
{
    return true;
}

// Ally-targeted, like Heal/Protection - Barzilla can also target himself.
bool Barzilla::isValidSpecialTarget(const BoardEntity& candidate) const
{
    return candidate.isAlive() && candidate.canBeTargetedBySpecial() && candidate.isAllyOf(getSide());
}

std::string Barzilla::getSpecialAbilityDescription() const
{
    return "Empowered Attack: Doubles the damage of a chosen ally's next attack.";
}

sf::Color Barzilla::getSpecialTargetHighlightColor() const
{
    return sf::Color(190, 90, 230, 180); // purple
}
