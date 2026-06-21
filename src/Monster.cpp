#include "Monster.h"
#include "Constants.h"
#include "TextureManager.h" // חובה בשביל לשלוף את התמונה!

Monster::Monster(const std::string& name, int health, int attackPower, int range, int cost, int q, int row, sf::Color color, const std::string& textureKey)
    : m_name(name), m_health(health), m_attackDamage(attackPower),
    m_range(range), m_cost(cost), m_q(q), m_row(row), m_color(color), m_textureKey(textureKey) // מעדכנים את הטקסטורה
{
}
void Monster::draw(sf::RenderWindow& window) const
{
    if (m_q == -1 && m_row == -1) return; // לא על הלוח

    // 1. ציור הילה צהובה אם נבחרה
    if (m_selected)
    {
        sf::CircleShape highlight(Config::MONSTER_BOARD_SIZE / 2.f + 5.f); // קצת יותר גדול מהמפלצת
        highlight.setOrigin({ highlight.getRadius(), highlight.getRadius() });
        highlight.setPosition(m_screenPos);
        highlight.setFillColor(sf::Color::Transparent);
        highlight.setOutlineThickness(3.f);
        highlight.setOutlineColor(sf::Color::Yellow);
        window.draw(highlight);
    }

    // 2. ציור המפלצת עצמה על הלוח
    try
    {
        const sf::Texture& texture = TextureManager::getInstance().getTexture(m_textureKey);
        sf::Sprite sprite(texture);
        sprite.setOrigin({ texture.getSize().x / 2.f, texture.getSize().y / 2.f });
        sprite.setPosition(m_screenPos);

        // --- הנוסחה החכמה לסקייל על הלוח ---
        // מתאים את התמונה אוטומטית לקוטר שהגדרנו בקבועים (44 פיקסלים)
        float maxTextureDim = std::max(texture.getSize().x, texture.getSize().y);
        float boardScale = Config::MONSTER_BOARD_SIZE / maxTextureDim;
        sprite.setScale({ boardScale, boardScale });
        // ------------------------------------

        window.draw(sprite);
    }
    catch (...)
    {
        // גיבוי בעיגול פשוט אם אין טקסטורה
        sf::CircleShape circle(Config::MONSTER_BOARD_SIZE / 2.f);
        circle.setFillColor(m_color);
        circle.setOrigin({ 16.f, 16.f }); // המפלצת ממורכזת!
        circle.setPosition(m_screenPos);
        window.draw(circle);
    }
}
bool Monster::contains(sf::Vector2f point, sf::Vector2f screenPos) const
{
    float dx = point.x - screenPos.x;
    float dy = point.y - screenPos.y;
    return (dx * dx + dy * dy) <= (32.f * 32.f);
}

bool Monster::isAlive() const
{
    return m_health > 0;
}

void Monster::takeDamage(int damage)
{
    m_health -= damage;
    if (m_health < 0) m_health = 0;
}


//void Monster::handleClick(const sf::Vector2f& pos)
//{
//
//}