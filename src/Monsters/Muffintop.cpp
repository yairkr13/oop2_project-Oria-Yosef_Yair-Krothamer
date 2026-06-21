#include "Monsters/Muffintop.h"
#include "TextureManager.h"

Muffintop::Muffintop()
    : Monster("Muffintop", BASE_HEALTH, BASE_ATTACK, BASE_RANGE, COST, -1, -1, sf::Color::Magenta, "muffintop")
{
    TextureManager::getInstance().loadTexture("muffintop", "resources/Monster/Muffintop/Muffintop.png");
    TextureManager::getInstance().loadTexture("muffintop_card", "resources/Monster/Muffintop/Muffintop_card.png");
}

void Muffintop::attack(Monster& target)
{
    target.takeDamage(m_attackDamage);
}
