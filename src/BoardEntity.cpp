#include "BoardEntity.h"
#include "Tile.h" // ה-include הזה מותר כאן!
//
//BoardEntity::~BoardEntity() {
//     קסם ה-OOP האוטומטי: 
//     רגע לפני שהישות נמחקת לגמרי מהזיכרון, אם היא יושבת על משבצת...
//    if (m_currentTile != nullptr) {
//         היא מנקה את עצמה מהמשבצת כדי שלא יישאר שם Dangling Pointer
//        m_currentTile->clearEntity();
//    }
//}

//BoardEntity::~BoardEntity() 
//{
//    if (m_currentTile != nullptr && m_currentTile->getEntity() == this)
//        m_currentTile->clearEntity();
//    //delete this;
//}

void BoardEntity::spawnOnBoard(int q, int row, const sf::Vector2f& screenPos) {
    m_q = q;
    m_row = row;
    m_screenPos = screenPos;
}

bool BoardEntity::isAlive() const
{
    return m_health > 0;
}

void BoardEntity::takeDamage(int damage)
{
    m_health -= damage;
    if (m_health < 0) m_health = 0;
}


void BoardEntity::drawHealthBar(sf::RenderWindow& window) const
{
    if (!isAlive()) return;
    /*if (m_side == currentPlayer)
        return;*/
        // 1. הגדרת מידות למד החיים (למשל, 80% מרוחב משבצת הלוח)
    float barWidth = Config::MONSTER_BOARD_SIZE * 0.8f;
    float barHeight = 6.f;

    // 2. חישוב מיקום מעל ראש המפלצת (בהנחה ש-m_screenPos הוא מרכז הדמות)
    float x = m_screenPos.x - (barWidth / 2.f);
    float y = m_screenPos.y - (Config::MONSTER_BOARD_SIZE / 2.f) - 12.f; // 12 פיקסלים מעל הדמות

    // 3. ציור רקע מד החיים (אדום כהה או אפור)
    sf::RectangleShape bgBar({ barWidth, barHeight });
    bgBar.setPosition({ x, y });
    bgBar.setFillColor(sf::Color(80, 20, 20)); // אדום עמוק

    // 4. חישוב יחס החיים שנותרו וציור המד הירוק
    float healthRatio = static_cast<float>(m_health) / m_maxHealth;
    if (healthRatio < 0.f) healthRatio = 0.f;

    sf::RectangleShape fgBar({ barWidth * healthRatio, barHeight });
    fgBar.setPosition({ x, y });
    fgBar.setFillColor(sf::Color(50, 220, 50)); // ירוק בהיר

    // 5. רישום על המסך
    window.draw(bgBar);
    window.draw(fgBar);
}