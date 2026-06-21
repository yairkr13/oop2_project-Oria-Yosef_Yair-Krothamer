#include "Tile.h"
const float TILE_RADIUS = 48.f;// Assuming each tile is 64x64 pixels, the radius would be 32


Tile::Tile(int q, int row, const sf::Vector2f& position)// <--- כאן אנחנו קוראים לבנאי של מחלקת האב!
    :m_q(q),
    m_row(row),
    m_isPassable(true)
{
    // הגדרות המשושה (m_shape)...

    m_shape.setRadius(TILE_RADIUS);

    // הקסם שקורה כאן: מעגל עם 6 נקודות הוא משושה!
    m_shape.setPointCount(6);

    // צבע פנימי
    m_shape.setFillColor(sf::Color(100, 250, 50)); // ירוק למשל

    // קו מתאר (מסגרת) כדי שנוכל לראות את הגבולות של כל משושה
    m_shape.setOutlineThickness(2.f);
    m_shape.setOutlineColor(sf::Color::Black);

    // ממקמים את המשושה
    //m_shape.setRotation(sf::degrees(30.f));
    m_shape.setPosition(position);
    //m_shape.setOrigin({ TILE_RADIUS, TILE_RADIUS });
    // אופציונלי: להגדיר את נקודת העוגן לאמצע המשושה
    // m_shape.setOrigin(radius, radius);
}

void Tile::draw(sf::RenderWindow& window) const
{
    window.draw(m_shape);
}

void Tile::setHighlighted(bool highlighted)
{
    m_isHighlighted = highlighted;
    if (highlighted)
    {
        m_shape.setFillColor(sf::Color(150, 255, 150)); // ירוק בהיר מסומן
        m_shape.setOutlineColor(sf::Color::White);      // מסגרת לבנה בולטת
    }
    else
    {
        m_shape.setFillColor(sf::Color(100, 250, 50));  // צבע מקורי שלכם
        m_shape.setOutlineColor(sf::Color::Black);
    }
}