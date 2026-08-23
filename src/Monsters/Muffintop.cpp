#include "Monsters/Muffintop.h"
#include "Attacks/SpinningProjectileAnimation.h"
#include "Attacks/RisingEffectAnimation.h"
#include "AssetsManager.h"
#include "Constants.h"

namespace
{
    constexpr float MUFFIN_SHOT_DURATION = 0.5f; // seconds for the muffin to fly from Muffintop to its target

    // On-screen size of the flying muffin, kept proportional to
    // Config::MONSTER_BOARD_SIZE - the same reference constant Monster's
    // own sprite is scaled against - rather than MuffinShot.png's raw pixel
    // size, so it reads as a projectile next to a full-size monster instead
    // of overwhelming it.
    constexpr float MUFFIN_SHOT_SIZE = Config::MONSTER_BOARD_SIZE * 0.6f;

    // Heal effect: rises the full height of a tile (bottom to top), sized
    // similarly to the other effect animations, over a gentler duration
    // than the sharper attack effects - a heal is meant to read as a slow
    // glow, not an impact.
    constexpr float HEAL_EFFECT_DURATION = 0.6f;
    constexpr float HEAL_EFFECT_SIZE = Config::MONSTER_BOARD_SIZE * 0.7f;
    constexpr float HEAL_EFFECT_RISE_DISTANCE = 2.f * Config::TILE_RADIUS;
}

Muffintop::Muffintop(PlayerSide side)
    : Monster(side, "Muffintop", BASE_HEALTH, BASE_ATTACK, BASE_RANGE, BASE_COOLDOWN, -1, -1, sf::Color::Magenta, "muffintop")
{
    //TextureManager::getInstance().loadTexture("muffintop_r", "resources/Monster/Muffintop/Muffintop_R.png");
    //TextureManager::getInstance().loadTexture("muffintop_l", "resources/Monster/Muffintop/Muffintop_L.png");
    //TextureManager::getInstance().loadTexture("muffintop_card_r", "resources/Monster/Muffintop/Muffintop_card_R.png");
    //TextureManager::getInstance().loadTexture("muffintop_card_l", "resources/Monster/Muffintop/Muffintop_card_L.png");
}


// Heal Ally: the Special commits here (Monster::useSpecialAbility has
// already consumed the action and reset the cooldown by the time this
// runs), but the actual +25% max-HP heal is deferred to the moment the
// HealEffect animation finishes rising - not applied immediately. The
// effect is created here (Muffintop knows its own ability's texture/style)
// but handed to the TARGET to own/update/draw (playSpecialAbilityAnimation),
// exactly mirroring how an attacker hands an attack animation to itself:
// here the effect's owner is whichever entity it visually belongs to, and
// that's the healed ally's own tile, not Muffintop.
void Muffintop::onSpecialAbility(Board& board, BoardEntity* target)
{
    Monster* targetMonster = target ? target->asMonster() : nullptr;
    if (!targetMonster) return;

    const sf::Texture& healEffectTexture = AssetsManager::getInstance().getTexture("heal_effect");

    // Starts at the bottom of the target's own tile and rises to the top -
    // getScreenPosition() is the tile's center, so the bottom is one
    // TILE_RADIUS below it.
    sf::Vector2f bottomOfTile = targetMonster->getScreenPosition() + sf::Vector2f(0.f, Config::TILE_RADIUS);

    auto healEffect = std::make_unique<RisingEffectAnimation>(
        healEffectTexture, bottomOfTile, HEAL_EFFECT_RISE_DISTANCE, HEAL_EFFECT_DURATION, HEAL_EFFECT_SIZE);

    // heal() (BoardEntity) still does the actual +HP and max-HP clamp -
    // nothing here duplicates that math, it only decides *when* to call it.
    healEffect->setOnImpact([targetMonster]() {
        targetMonster->heal(static_cast<int>(targetMonster->getMaxHealth() * 0.25f));
    });

    targetMonster->playSpecialAbilityAnimation(std::move(healEffect));
}

std::unique_ptr<AttackAnimation> Muffintop::createAttackAnimation(BoardEntity* target) const
{
    if (!target) return nullptr;

    // Genuinely different mechanism from Mozzy/Barzilla's splash-reveal:
    // the muffin itself flies from Muffintop to the target while spinning,
    // so this uses SpinningProjectileAnimation instead of SplashAttackAnimation.
    const sf::Texture& muffinShotTexture = AssetsManager::getInstance().getTexture("muffin_shot");
    return std::make_unique<SpinningProjectileAnimation>(
        muffinShotTexture, m_screenPos, target->getScreenPosition(), MUFFIN_SHOT_DURATION, MUFFIN_SHOT_SIZE);
}