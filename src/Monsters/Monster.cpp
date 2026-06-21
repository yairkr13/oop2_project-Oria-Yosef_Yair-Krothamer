#include "Monsters/Monster.h"
#include "Constants.h"
#include "TextureManager.h"

Monster::Monster(const std::string& name, int health, int attackPower, int range, int cost, int q, int row, sf::Color color, const std::string& textureKey)
    : m_name(name), m_health(health), m_attackDamage(attackPower),
    m_range(range), m_cost(cost), m_q(q), m_row(row), m_color(color), m_textureKey(textureKey)
{
}

void Monster::draw(sf::RenderWindow& window) const
{
    if (m_q == -1 && m_row == -1) return;

    try
    {
        const sf::Texture& texture = TextureManager::getInstance().getTexture(m_textureKey);
        sf::Sprite sprite(texture);
        sprite.setOrigin({ texture.getSize().x / 2.f, texture.getSize().y / 2.f });
        sprite.setPosition(m_screenPos);

        float maxTextureDim = std::max(texture.getSize().x, texture.getSize().y);
        float boardScale = Config::MONSTER_BOARD_SIZE / maxTextureDim;
        sprite.setScale({ boardScale, boardScale });

        window.draw(sprite);
    }
    catch (...)
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
