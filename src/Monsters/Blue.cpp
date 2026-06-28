#include "Monsters/Blue.h"
#include "TextureManager.h"

Blue::Blue(PlayerSide side)
    : Monster(side, "Blue", BASE_HEALTH, BASE_ATTACK, BASE_RANGE, COST, -1, -1, sf::Color::Magenta, "blue")
{
    //TextureManager::getInstance().loadTexture("blue_r", "resources/Monster/Blue/Blue_R.png");
    //TextureManager::getInstance().loadTexture("blue_l", "resources/Monster/Blue/Blue_L.png");
    //TextureManager::getInstance().loadTexture("blue_card_r", "resources/Monster/Blue/Blue_card_R.png");
    //TextureManager::getInstance().loadTexture("blue_card_l", "resources/Monster/Blue/Blue_card_L.png");
}
//
//void Blue::attack(Monster& target)
//{
//    target.takeDamage(m_attackDamage);
//}
