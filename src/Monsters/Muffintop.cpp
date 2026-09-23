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
    // glow, not an impact. Slowed down further from the original single-sprite
    // timing so the player has time to actually notice it.
    constexpr float HEAL_EFFECT_DURATION = 0.9f;
    constexpr float HEAL_EFFECT_SIZE = Config::MONSTER_BOARD_SIZE * 0.7f;
    constexpr float HEAL_EFFECT_RISE_DISTANCE = 2.f * Config::TILE_RADIUS;

    // Three copies (left/center/right) instead of one, so the heal reads as
    // a single wide bloom around the healed monster rather than one narrow
    // beam. Spacing is kept well under HEAL_EFFECT_SIZE so the three overlap
    // visually and stay one effect, not three separate ones; the small
    // stagger gives them a gentle ripple instead of rising in lockstep.
    constexpr int HEAL_EFFECT_INSTANCE_COUNT = 3;
    constexpr float HEAL_EFFECT_HORIZONTAL_SPACING = Config::MONSTER_BOARD_SIZE * 0.4f;
    constexpr float HEAL_EFFECT_STAGGER_DELAY = 0.06f;

    // Attack sprite sheet timing: the shared 6x4 grid (see
    // Monster::setStandardSpriteAnimations). Frame duration is chosen so
    // the full 24-frame cycle completes in ~0.48s - close to
    // MUFFIN_SHOT_DURATION (0.5s) above - so Muffintop's own windup-to-throw
    // motion finishes right around when the projectile actually lands,
    // instead of being cut off mid-cycle or finishing early and holding on
    // the last frame. isAttacking() (see Monster::update()/draw()) is what
    // drives this - already true for exactly the projectile's flight time,
    // so no extra timing state was needed here.
    constexpr float ATTACK_FRAME_DURATION = 0.02f;
}

Muffintop::Muffintop(PlayerSide side)
    : Monster(side, BASE_HEALTH, BASE_ATTACK, BASE_RANGE, BASE_COOLDOWN, -1, -1, sf::Color::Magenta, "muffintop")
{
    setStandardSpriteAnimations("muffintop", "muffintop_walk", ATTACK_FRAME_DURATION);
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
//void Muffintop::onSpecialAbility(Board& board, BoardEntity* target)
//{
//    Monster* targetMonster = target ? target->asMonster() : nullptr;
//    if (!targetMonster) return;
//
//    //��� ��� ���� �� ������� �������� ���� ����� ���� ��� �������?????
//    const sf::Texture& healEffectTexture = AssetsManager::getInstance().getTexture("heal_effect");
//
//    // Starts at the bottom of the target's own tile and rises to the top -
//    // getScreenPosition() is the tile's center, so the bottom is one
//    // TILE_RADIUS below it.
//    sf::Vector2f bottomOfTile = targetMonster->getScreenPosition() + sf::Vector2f(0.f, Config::TILE_RADIUS);
//
//    auto healEffect = std::make_unique<RisingEffectAnimation>(
//        healEffectTexture, bottomOfTile, HEAL_EFFECT_RISE_DISTANCE, HEAL_EFFECT_DURATION, HEAL_EFFECT_SIZE,
//        HEAL_EFFECT_INSTANCE_COUNT, HEAL_EFFECT_HORIZONTAL_SPACING, HEAL_EFFECT_STAGGER_DELAY);
//
//    // heal() (BoardEntity) still does the actual +HP and max-HP clamp -
//    // nothing here duplicates that math, it only decides *when* to call it.
//    healEffect->setOnImpact([targetMonster]() {
//        targetMonster->heal(static_cast<int>(targetMonster->getMaxHealth() * 0.25f));
//    });
//
//    targetMonster->playSpecialAbilityAnimation(std::move(healEffect));
//}
void Muffintop::onSpecialAbility(const Board& board, BoardEntity* target)
{
    //Monster* targetMonster = target ? target->asMonster() : nullptr;
    //if (!targetMonster) return;

    //��� ��� ���� �� ������� �������� ���� ����� ���� ��� �������?????
    const sf::Texture& healEffectTexture = AssetsManager::getInstance().getTexture("heal_effect");

    // Starts at the bottom of the target's own tile and rises to the top -
    // getScreenPosition() is the tile's center, so the bottom is one
    // TILE_RADIUS below it.
    sf::Vector2f bottomOfTile = target->getScreenPosition() + sf::Vector2f(0.f, Config::TILE_RADIUS);

    auto healEffect = std::make_unique<RisingEffectAnimation>(
        healEffectTexture, bottomOfTile, HEAL_EFFECT_RISE_DISTANCE, HEAL_EFFECT_DURATION, HEAL_EFFECT_SIZE,
        HEAL_EFFECT_INSTANCE_COUNT, HEAL_EFFECT_HORIZONTAL_SPACING, HEAL_EFFECT_STAGGER_DELAY);

    // heal() (BoardEntity) still does the actual +HP and max-HP clamp -
    // nothing here duplicates that math, it only decides *when* to call it.
    healEffect->setOnImpact([target]() {
        target->heal(static_cast<int>(target->getMaxHealth() * 0.25f));
        });

    target->playSpecialAbilityAnimation(std::move(healEffect));
}

std::unique_ptr<AttackAnimation> Muffintop::createAttackAnimation(sf::Vector2f targetPosition) const
{
    // Genuinely different mechanism from Mozzy/Barzilla's splash-reveal:
    // the muffin itself flies from Muffintop to the target while spinning,
    // so this uses SpinningProjectileAnimation instead of SplashAttackAnimation.
    const sf::Texture& muffinShotTexture = AssetsManager::getInstance().getTexture("muffin_shot");
    return std::make_unique<SpinningProjectileAnimation>(
        muffinShotTexture, m_screenPos, targetPosition, MUFFIN_SHOT_DURATION, MUFFIN_SHOT_SIZE);
}

bool Muffintop::specialAbilityNeedsTarget() const
{
    return true;
}

std::string Muffintop::getSpecialAbilityDescription() const
{
    return "Heal: Restores health to a targeted friendly monster.";
}

bool Muffintop::isValidSpecialTarget(const BoardEntity& candidate) const
{
    return candidate.isAlive() && candidate.canBeTargetedBySpecial() && candidate.isAllyOf(getSide());
}

float Muffintop::scoreAsSpecialTarget(const BoardEntity& candidate) const
{
    return static_cast<float>(candidate.getMaxHealth() - candidate.getHealth());
}

sf::Color Muffintop::getSpecialTargetHighlightColor() const
{
    return sf::Color(0, 100, 0, 180); // dark green
}