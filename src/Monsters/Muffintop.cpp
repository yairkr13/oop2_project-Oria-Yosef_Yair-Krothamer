#include "Monsters/Muffintop.h"
#include "TextureManager.h"

Muffintop::Muffintop()
    : Monster("Muffintop", BASE_HEALTH, BASE_ATTACK, BASE_RANGE, COST, -1, -1, sf::Color::Magenta, "muffintop")
{
    //TextureManager::getInstance().loadTexture("muffintop_r", "resources/Monster/Muffintop/Muffintop_R.png");
    //TextureManager::getInstance().loadTexture("muffintop_l", "resources/Monster/Muffintop/Muffintop_L.png");
    //TextureManager::getInstance().loadTexture("muffintop_card_r", "resources/Monster/Muffintop/Muffintop_card_R.png");
    //TextureManager::getInstance().loadTexture("muffintop_card_l", "resources/Monster/Muffintop/Muffintop_card_L.png");
}

//void Muffintop::attack(Monster& target)
//{
//    target.takeDamage(m_attackDamage);
//}
