#include "Monsters/Muffintop.h"
#include "Attacks/SpinningProjectileAnimation.h"
#include "Attacks/RisingEffectAnimation.h"
#include "AssetsManager.h"
#include "Constants.h"

namespace
{
    constexpr float MUFFIN_SHOT_DURATION = 0.5f; // seconds for the muffin to fly from Muffintop to its target

    // Proportional to Config::MONSTER_BOARD_SIZE, not MuffinShot.png's raw pixel size.
    constexpr float MUFFIN_SHOT_SIZE = Config::MONSTER_BOARD_SIZE * 0.6f;

    // Rises the height of a tile; slower and gentler than the attack
    // effects, reading as a heal rather than an impact.
    constexpr float HEAL_EFFECT_DURATION = 0.9f;
    constexpr float HEAL_EFFECT_SIZE = Config::MONSTER_BOARD_SIZE * 0.7f;
    constexpr float HEAL_EFFECT_RISE_DISTANCE = 2.f * Config::TILE_RADIUS;

    // Three staggered copies so the heal reads as one wide bloom, not a
    // single narrow beam.
    constexpr int HEAL_EFFECT_INSTANCE_COUNT = 3;
    constexpr float HEAL_EFFECT_HORIZONTAL_SPACING = Config::MONSTER_BOARD_SIZE * 0.4f;
    constexpr float HEAL_EFFECT_STAGGER_DELAY = 0.06f;

    // Attack sheet timing: matches MUFFIN_SHOT_DURATION's ~0.5s flight.
    constexpr float ATTACK_FRAME_DURATION = 0.02f;
}

Muffintop::Muffintop(PlayerSide side)
    : Monster(side, BASE_HEALTH, BASE_ATTACK, BASE_RANGE, BASE_COOLDOWN, -1, -1, sf::Color::Magenta, "muffintop")
{
    setStandardSpriteAnimations("muffintop", "muffintop_walk", ATTACK_FRAME_DURATION);
}


// Heal Ally: the actual +25% max-HP heal is deferred to the moment the
// HealEffect animation finishes rising, not applied immediately.
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

    const sf::Texture& healEffectTexture = AssetsManager::getInstance().getTexture("heal_effect");

    // Rises from the bottom of the target's tile - getScreenPosition() is
    // the tile's center, so the bottom is one TILE_RADIUS below it.
    sf::Vector2f bottomOfTile = target->getScreenPosition() + sf::Vector2f(0.f, Config::TILE_RADIUS);

    auto healEffect = std::make_unique<RisingEffectAnimation>(
        healEffectTexture, bottomOfTile, HEAL_EFFECT_RISE_DISTANCE, HEAL_EFFECT_DURATION, HEAL_EFFECT_SIZE,
        HEAL_EFFECT_INSTANCE_COUNT, HEAL_EFFECT_HORIZONTAL_SPACING, HEAL_EFFECT_STAGGER_DELAY);

    // heal() (BoardEntity) applies the actual +HP; this only decides when.
    healEffect->setOnImpact([target]() {
        target->heal(static_cast<int>(target->getMaxHealth() * 0.25f));
        });

    target->playSpecialAbilityAnimation(std::move(healEffect));
}

std::unique_ptr<AttackAnimation> Muffintop::createAttackAnimation(sf::Vector2f targetPosition) const
{
    // Unlike Mozzy/Barzilla's splash-reveal, the muffin itself flies and
    // spins toward the target.
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

// Ally-targeted, like Barzilla/Henrietta - flips the base enemy default.
bool Muffintop::isValidSpecialTarget(const BoardEntity& candidate) const
{
    return candidate.isAlive() && candidate.canBeTargetedBySpecial() && candidate.isAllyOf(getSide());
}

// Prefers whoever has the most missing HP.
float Muffintop::scoreAsSpecialTarget(const BoardEntity& candidate) const
{
    return static_cast<float>(candidate.getMaxHealth() - candidate.getHealth());
}

sf::Color Muffintop::getSpecialTargetHighlightColor() const
{
    return sf::Color(0, 100, 0, 180); // dark green
}
