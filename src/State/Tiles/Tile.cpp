#include "Tiles/Tile.h"
#include <limits>

Tile::Tile(int q, int row, const sf::Vector2f& position,const sf::Color& color)// <--- ��� ����� ������ ����� �� ����� ���!
    :m_q(q),
    m_row(row),
    m_isPassable(true),
    m_color(color)
{
    // ������ ������ (m_shape)...

    m_shape.setRadius(Config::TILE_RADIUS);

    // ���� ����� ���: ���� �� 6 ������ ��� �����!
    m_shape.setPointCount(6);

    // ��� �����
    m_shape.setFillColor(m_color); // light gray, semi-transparent

    m_shape.setOutlineThickness(2.f);
    m_shape.setOutlineColor(sf::Color(80, 80, 80, 180));

    // ������ �� ������
    m_shape.setPosition(position);
}

void Tile::draw(sf::RenderWindow& window) const
{
    window.draw(m_shape);
}

void Tile::drawEntity(sf::RenderWindow& window, PlayerSide currentSide) const
{
    if (m_entity != nullptr)
    {
        m_entity->draw(window, currentSide); // קריאה וירטואלית (אפס RTTI, מהיר לחלוטין)
    }
}

void Tile::setHighlighted(bool highlighted,const sf::Color& highlightColor)
{
    m_isHighlighted = highlighted;
    if (highlighted)
    {
        m_shape.setFillColor(ownHighlightColor().value_or(highlightColor)); // light green, semi-transparent (or this tile type's own fixed color)
        m_shape.setOutlineColor(sf::Color(200, 255, 200, 220));
    }
    else
    {
        m_shape.setFillColor(m_color); // light gray, semi-transparent
        m_shape.setOutlineColor(sf::Color(80, 80, 80, 180));
    }
}

void Tile::setEntity(BoardEntity* entity)
{
    m_entity = entity;
    m_isPassable = false;
}

void Tile::clearEntity()
{
    m_entity = nullptr;
    m_isPassable = true;
}

void Tile::receiveAttackFrom(BoardEntity* attacker)
{
    // No attacker, or nothing on this tile to hit.
    if (!attacker || m_entity == nullptr) return;

    BoardEntity* defender = m_entity;
    attacker->attack(defender);

    // isReadyForRemoval(), not isAlive() directly - a defender with a
    // configured death animation (see Monster::isReadyForRemoval) is
    // already dead (isAlive() is already false, so it's already
    // unselectable/untargetable/unable to act - see Tile::isOccupiedByEnemy/
    // Monster::canBeSelectedBy) but stays linked to this Tile so that
    // animation can keep playing; Board::update() clears it once it
    // finishes. A defender with no death animation configured is ready
    // immediately, same as before this distinction existed.
    if (defender->isReadyForRemoval())
    {
        clearEntity();
    }
}

void Tile::tickTurnBoundary()
{
    if (!m_entity) return;

    m_entity->onTurnBoundary();
    if (m_entity->isReadyForRemoval())
        clearEntity();
}

void Tile::updateEntity(float dt)
{
    if (!m_entity) return;

    m_entity->update(dt);
    // Re-check m_entity: update() could indirectly clear this tile (e.g. via a callback).
    if (m_entity && m_entity->isReadyForRemoval())
    {
        clearEntity();
    }
}

bool Tile::isEntityAnimating() const
{
    return m_entity && m_entity->isAnimating();
}

void Tile::damageEntity(int amount)
{
    if (!m_entity) return;

    m_entity->takeDamage(amount);
    if (m_entity->isReadyForRemoval())
        clearEntity();
}

float Tile::scoreAsAttackTarget() const
{
    return m_entity ? m_entity->scoreAsAttackTarget() : -std::numeric_limits<float>::infinity();
}