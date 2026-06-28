#include "Monsters/Barzilla.h"
#include "TextureManager.h"

Barzilla::Barzilla(PlayerSide side)
    : Monster(side, "Barzilla", BASE_HEALTH, BASE_ATTACK, BASE_RANGE, COST, -1, -1, sf::Color::Red, "barzilla")
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
