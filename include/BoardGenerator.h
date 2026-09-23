#pragma once
#include "Tiles/Tile.h"
#include <map>
#include <memory>
#include <utility>
#include <functional>
#include <random>

class Heart; // Only ever passed through as a pointer, for PanicPoint's constructor.

// Describes one board layout's shape and special-tile plan - the recipe
// BoardGenerator turns into an actual grid. Plain data, not a class
// hierarchy: every field is just a count/size a map author would tune, so
// there's nothing here that needs polymorphism yet.
struct BoardLayout
{
    int rows;
    int cols;

    int middleRow() const { return rows / 2; }
    int lavaTileCount;
    int holeTileCount;
    int panicPointCount; // expected 0 or 1, kept as a count for future flexibility

    // Columns on each side reserved for spawning, never eligible for a special tile.
    int spawnColumnWidth;
};

namespace BoardGenerator
{
    // Today's one standard layout (7 rows, 20 cols, 2 lava, 2 holes, 1 panic point).
    BoardLayout standardLayout();

    // Builds a plain hex grid of layout's shape (no special tiles yet). anchorToScreen
    // supplies each tile's screen position, so this carries no pixel knowledge itself.
    std::map<std::pair<int, int>, std::unique_ptr<Tile>> buildBaseGrid(
        const BoardLayout& layout,
        const std::function<sf::Vector2f(int, int)>& anchorToScreen);

    // Swaps lava/hole/panic-point tiles into grid in place, avoiding both spawn
    // margins. p1Heart/p2Heart are only forwarded for PanicPoint's constructor.
    // useFixedPlacement (PlayerVsRemote only) skips the RNG and uses a hardcoded
    // coordinate list so both peers land on the same board.
    void applySpecialTiles(
        std::map<std::pair<int, int>, std::unique_ptr<Tile>>& grid,
        const BoardLayout& layout,
        const std::function<sf::Vector2f(int, int)>& anchorToScreen,
        Heart* p1Heart, Heart* p2Heart,
        std::mt19937& rng, bool useFixedPlacement = false);
}
