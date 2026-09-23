#include "Tiles/LavaTile.h"

LavaTile::LavaTile(int q, int row, const sf::Vector2f& position)
    : Tile(q, row, position,sf::Color(250, 180, 180, 140))
{
}

void LavaTile::applyTileEffect()
{
    if (hasEntity())
    {
        if (getEntity()->canFly())
            return; // flying entities are unaffected
        damageEntity(20);
    }
}

std::optional<sf::Color> LavaTile::ownHighlightColor() const
{
    return sf::Color(255, 120, 80, 180);
}
