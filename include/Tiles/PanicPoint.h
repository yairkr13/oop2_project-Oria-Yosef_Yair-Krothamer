#pragma once
#include "Heart.h"
#include "Tiles/Tile.h"

// A hex tile that damages the opposing player's Heart whenever a monster
// stands on it.
class PanicPoint : public Tile
{
public:
    PanicPoint(int q, int row, const sf::Vector2f& position, Heart* p1Heart, Heart* p2Heart);
    void applyTileEffect() override;

protected:
    std::optional<sf::Color> ownHighlightColor() const override;
private:
    Heart* m_p1Heart; // damaged when a Right-side monster stands here
    Heart* m_p2Heart; // damaged when a Left-side monster stands here
};
