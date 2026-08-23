#include "Monsters/Barzilla.h"
#include "Attacks/SplashAttackAnimation.h"
#include "AssetsManager.h"
#include "Constants.h"

namespace
{
    constexpr float FIRE_BLAST_DURATION = 0.5f; // seconds for the blast to travel from Barzilla to its target

    // Same reasoning as Mozzy's ACID_SPLASH_THICKNESS: kept proportional to
    // Config::MONSTER_BOARD_SIZE (the project's existing on-board sizing
    // reference) rather than FireBlast.png's raw pixel height.
    constexpr float FIRE_BLAST_THICKNESS = Config::MONSTER_BOARD_SIZE * 0.5f;
}

Barzilla::Barzilla(PlayerSide side)
    : Monster(side, "Barzilla", BASE_HEALTH, BASE_ATTACK, BASE_RANGE, -1, -1, sf::Color::Red, "barzilla")
{
    //TextureManager::getInstance().loadTexture("barzilla_r", "resources/Monster/Barzilla/Barzilla_R.png");
    //TextureManager::getInstance().loadTexture("barzilla_l", "resources/Monster/Barzilla/Barzilla_L.png");
    //TextureManager::getInstance().loadTexture("barzilla_card_r", "resources/Monster/Barzilla/Barzilla_card_R.png");
    //TextureManager::getInstance().loadTexture("barzilla_card_l", "resources/Monster/Barzilla/Barzilla_card_L.png");
}
//
//void Barzilla::attack(std::shared_ptr<Monster> target)
//{
//    target.takeDamage(m_attackDamage);
//}

std::unique_ptr<AttackAnimation> Barzilla::createAttackAnimation(BoardEntity* target) const
{
    if (!target) return nullptr;

    // Same "grows/reveals from attacker toward target" mechanism as Mozzy's
    // acid splash - only the texture (and its proportions) differ, so this
    // reuses SplashAttackAnimation as-is rather than duplicating it.
    const sf::Texture& fireBlastTexture = AssetsManager::getInstance().getTexture("fire_blast");
    return std::make_unique<SplashAttackAnimation>(
        fireBlastTexture, m_screenPos, target->getScreenPosition(), FIRE_BLAST_DURATION, FIRE_BLAST_THICKNESS);
}
