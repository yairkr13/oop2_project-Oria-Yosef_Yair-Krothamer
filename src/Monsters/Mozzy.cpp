#include "Monsters/Mozzy.h"
#include "Attacks/SplashAttackAnimation.h"
#include "Attacks/FormingEffectAnimation.h"
#include "AssetsManager.h"
#include "Constants.h"

namespace
{
    constexpr float ACID_SPLASH_DURATION = 0.5f; // seconds for the splash to travel from Mozzy to its target

    // Proportional to Config::MONSTER_BOARD_SIZE, not AcidSplash.png's raw
    // pixel size (which reads far too thick on-board).
    constexpr float ACID_SPLASH_THICKNESS = Config::MONSTER_BOARD_SIZE * 0.5f;

    // Ice forms top-to-bottom over the target, reading as "freezing in
    // place" rather than an incoming projectile.
    constexpr float FREEZE_EFFECT_WIDTH = Config::MONSTER_BOARD_SIZE * 1.1f;
    constexpr float FREEZE_EFFECT_REVEAL_DURATION = 0.7f;
    constexpr float FREEZE_EFFECT_HOLD_DURATION = 0.4f;

    // Upward positioning nudge, applied to the whole effect.
    constexpr float FREEZE_EFFECT_VERTICAL_SHIFT = Config::MONSTER_BOARD_SIZE * 0.25f;

    // Attack sheet timing: matches AcidSplash's ~0.5s travel time.
    constexpr float ATTACK_FRAME_DURATION = 0.02f;
}

Mozzy::Mozzy(PlayerSide side)
    : Monster(side, BASE_HEALTH, BASE_ATTACK, BASE_RANGE, BASE_COOLDOWN, -1, -1, sf::Color::Cyan, "mozzy",true)
{
    // Flying monster: uses its fly sheet instead of a ground walk cycle.
    setStandardSpriteAnimations("mozzy", "mozzy_fly", ATTACK_FRAME_DURATION);
}

//void Mozzy::attack(Monster& target)
//{
//    target.takeDamage(m_attackDamage);
//}

std::unique_ptr<AttackAnimation> Mozzy::createAttackAnimation(sf::Vector2f targetPosition) const
{
    const sf::Texture& acidSplashTexture = AssetsManager::getInstance().getTexture("acid_splash");
    return std::make_unique<SplashAttackAnimation>(
        acidSplashTexture, m_screenPos, targetPosition, ACID_SPLASH_DURATION, ACID_SPLASH_THICKNESS);
}

// Freeze commits immediately and synchronously; the ice-forming effect
// below is purely visual, owned by the target.
void Mozzy::onSpecialAbility(const Board& board, BoardEntity* target)
{
    /*Monster* targetMonster = target ? target->asMonster() : nullptr;
    if (!targetMonster) return;*/
	if (!target->canBeTargetedBySpecial()) return;

    target->applyFreeze();

    const sf::Texture& freezeEffectTexture = AssetsManager::getInstance().getTexture("freeze_effect");

    // FormingEffectAnimation derives its own start/end path from this one point.
    sf::Vector2f freezeTargetPosition = target->getScreenPosition() - sf::Vector2f(0.f, FREEZE_EFFECT_VERTICAL_SHIFT);

    auto freezeEffect = std::make_unique<FormingEffectAnimation>(
        freezeEffectTexture, freezeTargetPosition,
        FREEZE_EFFECT_WIDTH, FREEZE_EFFECT_REVEAL_DURATION, FREEZE_EFFECT_HOLD_DURATION);

    target->playSpecialAbilityAnimation(std::move(freezeEffect));
}

std::string Mozzy::getSpecialAbilityDescription() const
{
    return "Freeze: Freezes an enemy monster, preventing it from taking actions during its next turn.";
}

bool Mozzy::specialAbilityNeedsTarget() const
{
    return true;
}

// Prefers the enemy with the most HP - the biggest ongoing threat.
float Mozzy::scoreAsSpecialTarget(const BoardEntity& candidate) const
{
    return static_cast<float>(candidate.getHealth());
}

sf::Color Mozzy::getSpecialTargetHighlightColor() const
{
    return sf::Color(255, 255, 255, 180); // white
}
