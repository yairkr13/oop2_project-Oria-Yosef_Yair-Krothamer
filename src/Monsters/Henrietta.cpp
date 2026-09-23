#include "Monsters/Henrietta.h"
#include "Attacks/BurstProjectileAnimation.h"
#include "Attacks/GrowingEffectAnimation.h"
#include "AssetsManager.h"
#include "Constants.h"

namespace
{
    // Normal attack (FlameWeb): same burst mechanism as Blue's WindBlast,
    // just a different texture.
    constexpr int FLAME_WEB_COUNT = 5;
    constexpr float FLAME_WEB_LAUNCH_INTERVAL = 0.08f;
    constexpr float FLAME_WEB_TRAVEL_DURATION = 0.35f;
    constexpr float FLAME_WEB_SIZE = Config::MONSTER_BOARD_SIZE * 0.35f;

    // Grows outward over the target's tile, holds, then fades - reads as
    // a barrier forming rather than an incoming attack.
    constexpr float SHIELD_EFFECT_SIZE = Config::TILE_RADIUS * 1.7f;
    constexpr float SHIELD_EFFECT_GROW_DURATION = 0.35f;
    constexpr float SHIELD_EFFECT_HOLD_DURATION = 0.3f;
    constexpr float SHIELD_EFFECT_FADE_DURATION = 0.25f;

    // Attack sheet timing: matches the FlameWeb burst's ~0.67s flight
    // window, same shape as Blue's WindBlast.
    constexpr float ATTACK_FRAME_DURATION = 0.028f;
}

Henrietta::Henrietta(PlayerSide side)
    : Monster(side, BASE_HEALTH, BASE_ATTACK, BASE_RANGE, BASE_COOLDOWN, -1, -1, sf::Color::Green, "henrietta")
{
    setStandardSpriteAnimations("henrietta", "henrietta_walk", ATTACK_FRAME_DURATION);
}

std::unique_ptr<AttackAnimation> Henrietta::createAttackAnimation(sf::Vector2f targetPosition) const
{
    const sf::Texture& flameWebTexture = AssetsManager::getInstance().getTexture("flame_web");
    return std::make_unique<BurstProjectileAnimation>(
        flameWebTexture, m_screenPos, targetPosition,
        FLAME_WEB_COUNT, FLAME_WEB_LAUNCH_INTERVAL, FLAME_WEB_TRAVEL_DURATION, FLAME_WEB_SIZE);
}

// The shield itself lives on the target (BoardEntity::applyProtection/
// takeDamage); this just grants it and plays a purely visual effect.
void Henrietta::onSpecialAbility(const Board& board, BoardEntity* target)
{
    if (!target) return;

    target->applyProtection();

    const sf::Texture& shieldEffectTexture = AssetsManager::getInstance().getTexture("shield_effect");
    auto shieldEffect = std::make_unique<GrowingEffectAnimation>(
        shieldEffectTexture, target->getScreenPosition(),
        SHIELD_EFFECT_SIZE, SHIELD_EFFECT_GROW_DURATION, SHIELD_EFFECT_HOLD_DURATION, SHIELD_EFFECT_FADE_DURATION);

    target->playSpecialAbilityAnimation(std::move(shieldEffect));
}

bool Henrietta::specialAbilityNeedsTarget() const
{
    return true;
}

// Ally-targeted; also requires canBeTargetedBySpecial() (Heart doesn't
// override it, so Protection can't target a Heart either).
bool Henrietta::isValidSpecialTarget(const BoardEntity& candidate) const
{
    return candidate.isAlive() && candidate.canBeTargetedBySpecial() && candidate.isAllyOf(m_side);
}

// Prefers whoever has the least HP - the most vulnerable, and so the most
// worth shielding.
float Henrietta::scoreAsSpecialTarget(const BoardEntity& candidate) const
{
    return static_cast<float>(candidate.getMaxHealth() - candidate.getHealth());
}

std::string Henrietta::getSpecialAbilityDescription() const
{
    return "Protection: Grants a shield to an ally, blocking all damage until the next turn.";
}

sf::Color Henrietta::getSpecialTargetHighlightColor() const
{
    return sf::Color(255, 165, 0, 180); // orange
}
