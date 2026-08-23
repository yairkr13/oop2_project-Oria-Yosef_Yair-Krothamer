#include "Monsters/Mozzy.h"
#include "Attacks/SplashAttackAnimation.h"
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
}

Mozzy::Mozzy(PlayerSide side)
    : Monster(side, "Mozzy", BASE_HEALTH, BASE_ATTACK, BASE_RANGE, BASE_COOLDOWN, -1, -1, sf::Color::Cyan, "mozzy",true)
{
    //TextureManager::getInstance().loadTexture("mozzy_r", "resources/Monster/Mozzy/Mozzy_R.png");
    //TextureManager::getInstance().loadTexture("mozzy_l", "resources/Monster/Mozzy/Mozzy_L.png");
    //TextureManager::getInstance().loadTexture("mozzy_card_r", "resources/Monster/Mozzy/Mozzy_card_R.png");
    //TextureManager::getInstance().loadTexture("mozzy_card_l", "resources/Monster/Mozzy/Mozzy_card_L.png");
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

void Mozzy::onSpecialAbility(Board& board, BoardEntity* target)
{
    if (Monster* targetMonster = target ? target->asMonster() : nullptr)
        targetMonster->applyFreeze();
}
