#include "Monsters/Henrietta.h"
#include "Attacks/BurstProjectileAnimation.h"
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
}

Henrietta::Henrietta(PlayerSide side)
    : Monster(side, "Henrietta", BASE_HEALTH, BASE_ATTACK, BASE_RANGE, BASE_COOLDOWN, -1, -1, sf::Color::Green, "henrietta")
{
    //TextureManager::getInstance().loadTexture("henrietta_r", "resources/Monster/Henrietta/Henrietta_R.png");
    //TextureManager::getInstance().loadTexture("henrietta_l", "resources/Monster/Henrietta/Henrietta_L.png");
    //TextureManager::getInstance().loadTexture("henrietta_card_r", "resources/Monster/Henrietta/Henrietta_card_R.png");
    //TextureManager::getInstance().loadTexture("henrietta_card_l", "resources/Monster/Henrietta/Henrietta_card_L.png");
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
// isProtected() before dealing damage.
void Henrietta::onSpecialAbility(Board& board, BoardEntity* target)
{
    if (target)
        target->applyProtection();
}