#include "Muffintop.h"

Muffintop::Muffintop()
// הוספנו את "muffintop" בסוף!
    : Monster("Muffintop", BASE_HEALTH, BASE_ATTACK, BASE_RANGE, COST, -1, -1, sf::Color::Magenta, "muffintop")
{
}

// --- הפונקציה החדשה ---
void Muffintop::attack(Monster& target)
{
    // משתמש בפונקציה הפנימית של המפלצת המותקפת כדי להוריד לה חיים
    target.takeDamage(m_attackDamage);
}