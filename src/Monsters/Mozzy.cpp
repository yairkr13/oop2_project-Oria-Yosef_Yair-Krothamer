#include "Monsters/Mozzy.h"
#include "TextureManager.h"

Mozzy::Mozzy()
    : Monster("Mozzy", BASE_HEALTH, BASE_ATTACK, BASE_RANGE, COST, -1, -1, sf::Color::Cyan, "mozzy")
{
    TextureManager::getInstance().loadTexture("mozzy_r", "resources/Monster/Mozzy/Mozzy_R.png");
    TextureManager::getInstance().loadTexture("mozzy_l", "resources/Monster/Mozzy/Mozzy_L.png");
    TextureManager::getInstance().loadTexture("mozzy_card_r", "resources/Monster/Mozzy/Mozzy_card_R.png");
    TextureManager::getInstance().loadTexture("mozzy_card_l", "resources/Monster/Mozzy/Mozzy_card_L.png");
}

//void Mozzy::attack(Monster& target)
//{
//    target.takeDamage(m_attackDamage);
//}
