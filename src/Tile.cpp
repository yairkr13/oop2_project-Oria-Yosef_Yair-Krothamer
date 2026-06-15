#include "Tile.h"
const int RADIUS = 32; // Assuming each tile is 64x64 pixels, the radius would be 32
Tile::Tile(int q, int row, const sf::Vector2f& position)// <--- כאן אנחנו קוראים לבנאי של מחלקת האב!
    :m_q(q),
    m_row(row),
    m_isPassable(true)
{
    // הגדרות המשושה (m_shape)...

    m_shape.setRadius(RADIUS);

    // הקסם שקורה כאן: מעגל עם 6 נקודות הוא משושה!
    m_shape.setPointCount(6);

    // צבע פנימי
    m_shape.setFillColor(sf::Color(100, 250, 50)); // ירוק למשל

    // קו מתאר (מסגרת) כדי שנוכל לראות את הגבולות של כל משושה
    m_shape.setOutlineThickness(2.f);
    m_shape.setOutlineColor(sf::Color::Black);

    // ממקמים את המשושה
    m_shape.setPosition(position);

    // אופציונלי: להגדיר את נקודת העוגן לאמצע המשושה
    // m_shape.setOrigin(radius, radius);
}

void Tile::draw(sf::RenderWindow& window) const
{
    window.draw(m_shape);
}

