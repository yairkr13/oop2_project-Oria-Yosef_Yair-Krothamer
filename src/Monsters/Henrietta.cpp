#include "Monsters/Henrietta.h"
#include "TextureManager.h"

Henrietta::Henrietta()
    : Monster("Henrietta", BASE_HEALTH, BASE_ATTACK, BASE_RANGE, COST, -1, -1, sf::Color::Green, "henrietta")
{
    TextureManager::getInstance().loadTexture("henrietta", "resources/Monster/Henrietta/Henrietta.png");
    TextureManager::getInstance().loadTexture("henrietta_card", "resources/Monster/Henrietta/Henrietta_card.png");
}

void Henrietta::attack(Monster& target)
{
    target.takeDamage(m_attackDamage);
}
