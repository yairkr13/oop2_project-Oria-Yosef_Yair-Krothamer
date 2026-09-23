#pragma once
#include "Tiles/Tile.h"

// A hex tile that damages whatever occupies it each turn, unless the
// occupant can fly.
class LavaTile : public Tile
{
public:
    LavaTile(int q, int row, const sf::Vector2f& position);

    virtual void applyTileEffect() override;

protected:
    std::optional<sf::Color> ownHighlightColor() const override;
};
