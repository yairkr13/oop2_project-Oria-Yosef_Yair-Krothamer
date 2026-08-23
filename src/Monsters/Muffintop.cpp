#include "Monsters/Muffintop.h"
#include "Attacks/SpinningProjectileAnimation.h"
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
}

Muffintop::Muffintop(PlayerSide side)
    : Monster(side, "Muffintop", BASE_HEALTH, BASE_ATTACK, BASE_RANGE, BASE_COOLDOWN, -1, -1, sf::Color::Magenta, "muffintop")
{
    //TextureManager::getInstance().loadTexture("muffintop_r", "resources/Monster/Muffintop/Muffintop_R.png");
    //TextureManager::getInstance().loadTexture("muffintop_l", "resources/Monster/Muffintop/Muffintop_L.png");
    //TextureManager::getInstance().loadTexture("muffintop_card_r", "resources/Monster/Muffintop/Muffintop_card_R.png");
    //TextureManager::getInstance().loadTexture("muffintop_card_l", "resources/Monster/Muffintop/Muffintop_card_L.png");
}


// Heal Ally: 25% of the target's own MAX HP (not current HP - a flat,
// predictable heal, not one that shrinks in value the more it's needed).
// heal() itself (BoardEntity) already clamps to max HP, so no separate cap
// check is needed here.
void Muffintop::onSpecialAbility(Board& board, BoardEntity* target)
{
    if (Monster* targetMonster = target ? target->asMonster() : nullptr)
        targetMonster->heal(static_cast<int>(targetMonster->getMaxHealth() * 0.25f));
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