#include "Monsters/Henrietta.h"
#include "Attacks/BurstProjectileAnimation.h"
#include "Attacks/GrowingEffectAnimation.h"
#include "AssetsManager.h"
#include "Constants.h"

namespace
{
    // Normal-attack burst (FlameWeb): same reusable mechanism as Blue's
    // WindBlast, only the texture differs - see BurstProjectileAnimation.
    constexpr int FLAME_WEB_COUNT = 5;
    constexpr float FLAME_WEB_LAUNCH_INTERVAL = 0.08f;
    constexpr float FLAME_WEB_TRAVEL_DURATION = 0.35f;
    constexpr float FLAME_WEB_SIZE = Config::MONSTER_BOARD_SIZE * 0.35f;

    // Protection shield: grows outward from its own center over the target's
    // tile, holds briefly at full size once formed, then fades out - reads
    // as a barrier forming rather than an incoming attack. Sized close to a
    // full tile so it clearly surrounds the target without dwarfing it.
    constexpr float SHIELD_EFFECT_SIZE = Config::TILE_RADIUS * 1.7f;
    constexpr float SHIELD_EFFECT_GROW_DURATION = 0.35f;
    constexpr float SHIELD_EFFECT_HOLD_DURATION = 0.3f;
    constexpr float SHIELD_EFFECT_FADE_DURATION = 0.25f;

    // Walking sprite sheet: 6 columns x 4 rows (24 frames total), read row
    // by row left-to-right - verified against the actual file, same layout
    // as Muffintop's working sheet. Same frame duration as Muffintop for a
    // consistent pace across monsters.
    constexpr int WALK_SHEET_COLUMNS = 6;
    constexpr int WALK_SHEET_ROWS = 4;
    constexpr float WALK_FRAME_DURATION = 0.06f;
}

Henrietta::Henrietta(PlayerSide side)
    : Monster(side, "Henrietta", BASE_HEALTH, BASE_ATTACK, BASE_RANGE, BASE_COOLDOWN, -1, -1, sf::Color::Green, "henrietta")
{
    setWalkAnimation("henrietta_walk", WALK_SHEET_COLUMNS, WALK_SHEET_ROWS, WALK_FRAME_DURATION);
}

std::unique_ptr<AttackAnimation> Henrietta::createAttackAnimation(BoardEntity* target) const
{
    if (!target) return nullptr;

    const sf::Texture& flameWebTexture = AssetsManager::getInstance().getTexture("flame_web");
    return std::make_unique<BurstProjectileAnimation>(
        flameWebTexture, m_screenPos, target->getScreenPosition(),
        FLAME_WEB_COUNT, FLAME_WEB_LAUNCH_INTERVAL, FLAME_WEB_TRAVEL_DURATION, FLAME_WEB_SIZE);
}

// Protection: the affected entity encapsulates its own "cannot currently
// receive damage" state (see BoardEntity::applyProtection/takeDamage) -
// Henrietta just grants it, nothing here or anywhere else ever checks
// isProtected() before dealing damage. Gameplay is untouched and still
// commits synchronously, right here, exactly as before; only a purely
// visual shield effect is added on top, owned by the TARGET (not
// Henrietta) via the same playSpecialAbilityAnimation slot Muffintop's
// Heal and Mozzy's Freeze effects already use.
void Henrietta::onSpecialAbility(Board& board, BoardEntity* target)
{
    if (!target) return;

    target->applyProtection();

    const sf::Texture& shieldEffectTexture = AssetsManager::getInstance().getTexture("shield_effect");
    auto shieldEffect = std::make_unique<GrowingEffectAnimation>(
        shieldEffectTexture, target->getScreenPosition(),
        SHIELD_EFFECT_SIZE, SHIELD_EFFECT_GROW_DURATION, SHIELD_EFFECT_HOLD_DURATION, SHIELD_EFFECT_FADE_DURATION);

    target->playSpecialAbilityAnimation(std::move(shieldEffect));
}