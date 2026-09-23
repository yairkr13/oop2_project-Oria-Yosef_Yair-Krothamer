#pragma once
#include "Tiles/Tile.h"

class LavaTile : public Tile
{
public:
    LavaTile(int q, int row, const sf::Vector2f& position);

    virtual void applyTileEffect() override;

protected:
    std::optional<sf::Color> ownHighlightColor() const override;
};