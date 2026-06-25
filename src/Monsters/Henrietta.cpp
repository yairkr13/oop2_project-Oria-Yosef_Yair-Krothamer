#include "Monsters/Henrietta.h"
#include "TextureManager.h"

Henrietta::Henrietta()
    : Monster("Henrietta", BASE_HEALTH, BASE_ATTACK, BASE_RANGE, COST, -1, -1, sf::Color::Green, "henrietta")
{
    //TextureManager::getInstance().loadTexture("henrietta_r", "resources/Monster/Henrietta/Henrietta_R.png");
    //TextureManager::getInstance().loadTexture("henrietta_l", "resources/Monster/Henrietta/Henrietta_L.png");
    //TextureManager::getInstance().loadTexture("henrietta_card_r", "resources/Monster/Henrietta/Henrietta_card_R.png");
    //TextureManager::getInstance().loadTexture("henrietta_card_l", "resources/Monster/Henrietta/Henrietta_card_L.png");
}
//
//void Henrietta::attack(Monster& target)
//{
//    target.takeDamage(m_attackDamage);
//}
