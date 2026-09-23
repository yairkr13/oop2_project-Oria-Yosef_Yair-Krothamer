#include "Tiles/PanicPoint.h"
#include "Heart.h"

PanicPoint::PanicPoint(int q, int row, const sf::Vector2f& position, Heart* p1Heart, Heart* p2Heart)
    : Tile(q, row, position, sf::Color(200, 100, 250, 140)),
    m_p1Heart(p1Heart), m_p2Heart(p2Heart)
{
}

void PanicPoint::applyTileEffect()
{
    if (hasEntity())
    {
        auto entity = getEntity();

        // Damages the OPPOSING player's Heart, not the occupant's own side's.
        if (entity->getSide() == PlayerSide::Left && m_p2Heart)
            m_p2Heart->takeDamage(10);
        else if (entity->getSide() == PlayerSide::Right && m_p1Heart)
            m_p1Heart->takeDamage(10);
    }
}

std::optional<sf::Color> PanicPoint::ownHighlightColor() const
{
    return sf::Color(200, 100, 250, 180);
}
