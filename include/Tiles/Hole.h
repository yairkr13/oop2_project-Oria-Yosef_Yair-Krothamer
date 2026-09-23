#pragma once
#include "Tiles/Tile.h"

// A hex tile with no floor: impassable to ground monsters, but flying
// monsters can cross it freely.
class Hole : public Tile
{
public:
    Hole(int q, int row, const sf::Vector2f& position);
    bool isPassableFor(const BoardEntity* entity) const override;
protected:
    std::optional<sf::Color> ownHighlightColor() const override;
};
