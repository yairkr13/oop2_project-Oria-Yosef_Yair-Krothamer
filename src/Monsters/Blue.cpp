#include "Monsters/Blue.h"
#include "TextureManager.h"

Blue::Blue()
    : Monster("Blue", BASE_HEALTH, BASE_ATTACK, BASE_RANGE, COST, -1, -1, sf::Color::Magenta, "blue")
{
    TextureManager::getInstance().loadTexture("blue", "resources/Monster/Blue/Blue.png");
    TextureManager::getInstance().loadTexture("blue_card", "resources/Monster/Blue/Blue_card.png");
}

void Blue::attack(Monster& target)
{
    target.takeDamage(m_attackDamage);
}
