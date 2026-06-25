#include "Monsters/Monster.h"
#include "Constants.h"
#include "TextureManager.h"

Monster::Monster(const std::string& name, int health, int attackPower, int range, int cost, int q, int row, sf::Color color, const std::string& textureKey)
    : m_name(name), m_health(health), m_attackDamage(attackPower),
    m_range(range), m_cost(cost), m_q(q), m_row(row), m_color(color), m_textureKey(textureKey),
	m_sprite(TextureManager::getInstance().get<sf::Texture>(m_textureKey))
{
    try
    {
        const sf::Texture& texture = m_sprite.getTexture();
        //m_sprite(texture);

        // הגדרת נקודת המרכז
        m_sprite.setOrigin({ texture.getSize().x / 2.f, texture.getSize().y / 2.f});

        // חישוב ושמירת קנה המידה (Scale)
        float maxTextureDim = std::max(static_cast<float>(texture.getSize().x), static_cast<float>(texture.getSize().y));
        m_baseScale = Config::MONSTER_BOARD_SIZE / maxTextureDim;

        //m_sprite.setScale({ m_baseScale, m_baseScale });
        m_hasTexture = true;
    }
    catch (...)
    {
        // אם הטקסטורה חסרה, נדליק דגל שיגיד ל-draw לצייר מעגל צבעוני
        m_hasTexture = false;
    }
}

void Monster::draw(sf::RenderWindow& window) const
{
    if (m_q == -1 && m_row == -1) return;

    if(m_hasTexture)
    {
        m_sprite.setPosition(m_screenPos);

        float currentScaleX = (m_side == PlayerSide::Right) ? -m_baseScale : m_baseScale;
        m_sprite.setScale({ currentScaleX, m_baseScale });

        window.draw(m_sprite);
    }
    else
    {
        sf::CircleShape circle(Config::MONSTER_BOARD_SIZE / 2.f);
        circle.setFillColor(m_color);
        circle.setOrigin({ 16.f, 16.f });
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

void Monster::walkTo(const sf::Vector2f& targetScreenPos)
{
    m_targetPos = targetScreenPos;
    m_isMoving = true;
}

void Monster::update(float dt)
{
    if (!m_isMoving) return;

    float dx = m_targetPos.x - m_screenPos.x;
    float dy = m_targetPos.y - m_screenPos.y;
    float distance = std::sqrt(dx * dx + dy * dy);

    if (distance < 5.f)
    {
        m_screenPos = m_targetPos;
        m_isMoving = false;
    }
    else
    {
        m_screenPos.x += (dx / distance) * m_speed * dt;
        m_screenPos.y += (dy / distance) * m_speed * dt;
    }
}

//
//void Monster::attack(std::shared_ptr<Monster> target)
//{
//    target->takeDamage(m_attackDamage);
//}
