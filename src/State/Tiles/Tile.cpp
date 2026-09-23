#include "Tiles/Tile.h"
#include <limits>

Tile::Tile(int q, int row, const sf::Vector2f& position,const sf::Color& color)
    :m_q(q),
    m_row(row),
    m_isPassable(true),
    m_color(color)
{
    m_shape.setRadius(Config::TILE_RADIUS);
    m_shape.setPointCount(6);
    m_shape.setFillColor(m_color);

    m_shape.setOutlineThickness(2.f);
    m_shape.setOutlineColor(sf::Color(80, 80, 80, 180)); // gray

    m_shape.setPosition(position);
}

void Tile::draw(sf::RenderWindow& window) const
{
    window.draw(m_shape);
}

// Draws only the occupant, not the hex - split from draw() so Board can
// paint every tile's hex first, then every occupant, in two full passes
// (keeps layering consistent regardless of m_grid's iteration order).
void Tile::drawEntity(sf::RenderWindow& window, PlayerSide currentSide) const
{
    if (m_entity != nullptr)
    {
        m_entity->draw(window, currentSide);
    }
}

void Tile::setHighlighted(bool highlighted,const sf::Color& highlightColor)
{
    m_isHighlighted = highlighted;
    if (highlighted)
    {
        m_shape.setFillColor(ownHighlightColor().value_or(highlightColor));
        m_shape.setOutlineColor(sf::Color(200, 255, 200, 220)); // light green
    }
    else
    {
        m_shape.setFillColor(m_color);
        m_shape.setOutlineColor(sf::Color(80, 80, 80, 180)); // gray
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

    // isReadyForRemoval(), not isAlive(): a defender with a death animation
    // stays linked to this tile until that animation finishes.
    if (defender->isReadyForRemoval())
    {
        clearEntity();
    }
}

// No-op if this tile is empty.
void Tile::tickTurnBoundary()
{
    if (!m_entity) return;

    m_entity->onTurnBoundary();
    if (m_entity->isReadyForRemoval())
        clearEntity();
}

// No-op if this tile is empty.
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

// Environmental damage source (e.g. LavaTile) - same cleanup as receiveAttackFrom.
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

int Tile::getQ() const
{
    return m_q;
}

int Tile::getRow() const
{
    return m_row;
}

// m_shape's origin is its bounding-box corner, not its center - this
// corrects for that to return the tile's true screen-space center.
sf::Vector2f Tile::getScreenPosition() const
{
    return m_shape.getPosition() + sf::Vector2f(Config::TILE_RADIUS, Config::TILE_RADIUS);
}

bool Tile::isHighlighted() const
{
    return m_isHighlighted;
}

const BoardEntity* Tile::getEntity() const
{
    return m_entity;
}

// Mutable access, for the few callers (AIPlayer, GameplayState) that need
// to change the occupant itself, not just query it.
BoardEntity* Tile::getMutableEntity() const
{
    return m_entity;
}

bool Tile::hasEntity() const
{
    return m_entity != nullptr;
}

bool Tile::isPassableFor(const BoardEntity* entity) const
{
    return m_isPassable;
}

// Requires the occupant to be alive - a dying entity stays Tile-linked
// (death animation) but must not be a valid attack/special target.
bool Tile::isOccupiedByEnemy(PlayerSide mySide) const
{
    return isEntityAlive() && m_entity->isEnemyOf(mySide);
}

bool Tile::isOccupiedByAlly(PlayerSide mySide) const
{
    return isEntityAlive() && m_entity->isAllyOf(mySide);
}

void Tile::applyTileEffect()
{
}

// nullopt means "use whatever color the caller passed to setHighlighted()".
std::optional<sf::Color> Tile::ownHighlightColor() const
{
    return std::nullopt;
}

bool Tile::isEntityAlive() const
{
    return m_entity != nullptr && m_entity->isAlive();
}
