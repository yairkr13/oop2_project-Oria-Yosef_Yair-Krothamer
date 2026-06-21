#include "Monsters/Mozzy.h"
#include "TextureManager.h"

Mozzy::Mozzy()
    : Monster("Mozzy", BASE_HEALTH, BASE_ATTACK, BASE_RANGE, COST, -1, -1, sf::Color::Cyan, "mozzy")
{
    TextureManager::getInstance().loadTexture("mozzy", "resources/Monster/Mozzy/Mozzy.png");
    TextureManager::getInstance().loadTexture("mozzy_card", "resources/Monster/Mozzy/Mozzy_card.png");
}

void Mozzy::attack(Monster& target)
{
    target.takeDamage(m_attackDamage);
}
