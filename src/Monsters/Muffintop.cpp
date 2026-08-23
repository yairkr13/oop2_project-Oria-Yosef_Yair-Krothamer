#include "Monsters/Muffintop.h"

Muffintop::Muffintop(PlayerSide side)
    : Monster(side, "Muffintop", BASE_HEALTH, BASE_ATTACK, BASE_RANGE, -1, -1, sf::Color::Magenta, "muffintop")
{
    //TextureManager::getInstance().loadTexture("muffintop_r", "resources/Monster/Muffintop/Muffintop_R.png");
    //TextureManager::getInstance().loadTexture("muffintop_l", "resources/Monster/Muffintop/Muffintop_L.png");
    //TextureManager::getInstance().loadTexture("muffintop_card_r", "resources/Monster/Muffintop/Muffintop_card_R.png");
    //TextureManager::getInstance().loadTexture("muffintop_card_l", "resources/Monster/Muffintop/Muffintop_card_L.png");
}


//bool Muffintop::useSpecialAbility(BoardEntity* target)
//{
//    if (!isSpecialReady() || !target) return false;
//
//    target->takeDamage(m_attackDamage * 2); // דוגמה - יכולת ייחודית
//    m_specialCooldown = 5; // איפוס cooldown
//    return true;
//}

void Muffintop::onSpecialAbility(BoardEntity* target)
{
    // דוגמה לחיזוק עצמי: תוספת כוח התקפה לתור הנוכחי
    m_attackDamage += m_attackDamage*0.5; // או שדה מסוים של המפלצת
}