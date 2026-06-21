#include "Muffintop.h"

Muffintop::Muffintop()
    : Monster("Muffintop", BASE_HEALTH, BASE_ATTACK, BASE_RANGE, COST, -1, -1, sf::Color::Magenta, "muffintop")
{
}

void Muffintop::attack(Monster& target)
{
    target.takeDamage(m_attackDamage);
}