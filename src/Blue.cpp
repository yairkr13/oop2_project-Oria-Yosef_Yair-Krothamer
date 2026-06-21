#include "Blue.h"

Blue::Blue()
// הוספנו את "muffintop" בסוף!
    : Monster("Blue", BASE_HEALTH, BASE_ATTACK, BASE_RANGE, COST, -1, -1, sf::Color::Magenta, "blue")
{
}

// --- הפונקציה החדשה ---
void Blue::attack(Monster& target)
{
    // משתמש בפונקציה הפנימית של המפלצת המותקפת כדי להוריד לה חיים
    target.takeDamage(m_attackDamage);
}