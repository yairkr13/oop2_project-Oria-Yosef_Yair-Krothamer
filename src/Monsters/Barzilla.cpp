#include "Monsters/Barzilla.h"
#include "TextureManager.h"

Barzilla::Barzilla()
    : Monster("Barzilla", BASE_HEALTH, BASE_ATTACK, BASE_RANGE, COST, -1, -1, sf::Color::Red, "barzilla")
{
    TextureManager::getInstance().loadTexture("barzilla", "resources/Monster/Barzilla/Barzilla.png");
    TextureManager::getInstance().loadTexture("barzilla_card", "resources/Monster/Barzilla/Barzilla_card.png");
}

void Barzilla::attack(Monster& target)
{
    target.takeDamage(m_attackDamage);
}
