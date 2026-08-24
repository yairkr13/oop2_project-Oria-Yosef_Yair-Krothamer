#include "Monsters/Mozzy.h"
#include "Attacks/SplashAttackAnimation.h"
#include "Attacks/FormingEffectAnimation.h"
#include "AssetsManager.h"
#include "Constants.h"

namespace
{
    constexpr float ACID_SPLASH_DURATION = 0.5f; // seconds for the splash to travel from Mozzy to its target

    // On-screen thickness of the splash band, kept proportional to the
    // existing on-board sizing rather than the raw AcidSplash.png pixel
    // size (2149x732 - using its native height directly made the splash
    // roughly 15x thicker than a monster). Config::MONSTER_BOARD_SIZE (44)
    // is the project's own reference for "how big is something on the
    // board", the same constant every monster sprite is already scaled
    // against (see Monster::Monster) - half of it reads as a visible but
    // not overpowering band next to a full-size monster.
    constexpr float ACID_SPLASH_THICKNESS = Config::MONSTER_BOARD_SIZE * 0.5f;

    // Freeze effect: ice forms top-to-bottom over the target rather than
    // flying in, so it reads as "freezing in place" rather than an
    // incoming projectile. Width (not the texture's own largest dimension -
    // see FormingEffectAnimation) is set a bit wider than a monster's own
    // sprite so the ice visibly spans/encases it rather than reading as a
    // narrow sliver next to it; FreezeEffect.png is a tall image, so the
    // resulting height comes out taller than the width, which reads fine
    // for ice descending over a monster. Reveal is slow enough to actually
    // notice, then holds fully-formed briefly before disappearing.
    constexpr float FREEZE_EFFECT_WIDTH = Config::MONSTER_BOARD_SIZE * 1.1f;
    constexpr float FREEZE_EFFECT_REVEAL_DURATION = 0.7f;
    constexpr float FREEZE_EFFECT_HOLD_DURATION = 0.4f;

    // Small upward nudge applied to the whole effect (start and end alike) -
    // a pure positioning tweak, independent of size/duration above.
    constexpr float FREEZE_EFFECT_VERTICAL_SHIFT = Config::MONSTER_BOARD_SIZE * 0.25f;

    // Walking/movement sprite sheet: Mozzy is a flying monster, so its
    // MozzyFly.png sheet is used for its board-travel animation instead of
    // a ground walk cycle - same mechanism either way. 6 columns x 4 rows
    // (24 frames), verified against the actual file, same layout as
    // Muffintop's working sheet.
    constexpr int WALK_SHEET_COLUMNS = 6;
    constexpr int WALK_SHEET_ROWS = 4;
    constexpr float WALK_FRAME_DURATION = 0.06f;
}

Mozzy::Mozzy(PlayerSide side)
    : Monster(side, "Mozzy", BASE_HEALTH, BASE_ATTACK, BASE_RANGE, BASE_COOLDOWN, -1, -1, sf::Color::Cyan, "mozzy",true)
{
    setWalkAnimation("mozzy_fly", WALK_SHEET_COLUMNS, WALK_SHEET_ROWS, WALK_FRAME_DURATION);
}

//void Mozzy::attack(Monster& target)
//{
//    target.takeDamage(m_attackDamage);
//}

std::unique_ptr<AttackAnimation> Mozzy::createAttackAnimation(BoardEntity* target) const
{
    if (!target) return nullptr;

    const sf::Texture& acidSplashTexture = AssetsManager::getInstance().getTexture("acid_splash");
    return std::make_unique<SplashAttackAnimation>(
        acidSplashTexture, m_screenPos, target->getScreenPosition(), ACID_SPLASH_DURATION, ACID_SPLASH_THICKNESS);
}

// Freeze: gameplay is untouched and still commits synchronously, right
// here, exactly as before - applyFreeze() zeroes the target's actions
// immediately (see Monster::applyFreeze), independent of however long the
// visual effect below takes to play out. Only a purely-visual "ice forming"
// effect is added on top, owned by the TARGET (not Mozzy) via the same
// playSpecialAbilityAnimation slot Muffintop's Heal effect already uses.
void Mozzy::onSpecialAbility(Board& board, BoardEntity* target)
{
    Monster* targetMonster = target ? target->asMonster() : nullptr;
    if (!targetMonster) return;

    targetMonster->applyFreeze();

    const sf::Texture& freezeEffectTexture = AssetsManager::getInstance().getTexture("freeze_effect");

    // Pass the target's own screen position (nudged up slightly) -
    // FormingEffectAnimation derives where the reveal starts (above it)
    // itself, from the texture's actual scaled size, so shifting this one
    // point up shifts the whole start-to-end path up with it.
    sf::Vector2f freezeTargetPosition = targetMonster->getScreenPosition() - sf::Vector2f(0.f, FREEZE_EFFECT_VERTICAL_SHIFT);

    auto freezeEffect = std::make_unique<FormingEffectAnimation>(
        freezeEffectTexture, freezeTargetPosition,
        FREEZE_EFFECT_WIDTH, FREEZE_EFFECT_REVEAL_DURATION, FREEZE_EFFECT_HOLD_DURATION);

    targetMonster->playSpecialAbilityAnimation(std::move(freezeEffect));
}
