#pragma once
#include "Tiles/Tile.h"
#include <map>
#include <memory>
#include <utility>
#include <functional>
#include <random>

class Heart; // Forward declaration - only ever passed through as a pointer, for PanicPoint's constructor.

// Describes one board layout's shape and special-tile plan - the "recipe"
// BoardGenerator::build below turns into an actual grid. A future second
// map/stage is a second BoardLayout value (e.g. a second free function
// alongside standardLayout() below, or numbers read from a config file
// later) - Board itself never needs to change to support it, and neither
// does BoardGenerator.
//
// Deliberately a plain data struct, not a class hierarchy: every field here
// is a count/size a map author would actually want to tune, and today's
// (and any near-future) variation between maps is fully expressible as
// "different numbers", not "different behavior" - so there's nothing here
// that genuinely needs polymorphism yet. If a future map ever needs an
// actual new RULE (not just different numbers/shape), that's the point to
// grow this - not before.
struct BoardLayout
{
    int rows;
    int cols;

    // The board's own vertical center - depends only on rows, which this
    // struct already owns, so it computes it itself rather than a caller
    // reaching into `rows` and dividing by hand (and risking a hardcoded
    // literal drifting out of sync with the actual layout - see
    // Board::initPlayerHearts, which used to hardcode this).
    int middleRow() const { return rows / 2; }
    int lavaTileCount;
    int holeTileCount;
    int panicPointCount; // expected to be 0 or 1 - kept as a count rather
                          // than a bool so a future map could ask for more
                          // without this struct needing to change again.

    // How many columns on each side are reserved for spawning and therefore
    // never eligible for a special tile - see BoardGenerator::applySpecialTiles
    // and Board::spawnColumnRange, both of which derive their own column
    // band from this same field instead of hardcoding "2 columns", so
    // spawn placement, spawn highlighting, and special-tile placement all
    // stay in sync with whatever `cols` a layout actually uses.
    int spawnColumnWidth;
};

namespace BoardGenerator
{
    // Today's one standard layout - the exact shape/tile counts Board has
    // always used (7 rows, 14 columns, 2 lava, 2 holes, 1 panic point, a
    // 2-column spawn margin on each side).
    BoardLayout standardLayout();

    // Builds a plain hex grid of `layout`'s shape (no special tiles yet -
    // see applySpecialTiles below) - `anchorToScreen` supplies each tile's
    // screen position for a given (q, row), so this function carries no
    // pixel-position knowledge of its own; Board hands in its own
    // tileAnchor() as that callback.
    std::map<std::pair<int, int>, std::unique_ptr<Tile>> buildBaseGrid(
        const BoardLayout& layout,
        const std::function<sf::Vector2f(int, int)>& anchorToScreen);

    // Swaps `layout.lavaTileCount` LavaTiles, `layout.holeTileCount`
    // Holes, and (if `layout.panicPointCount > 0`) that many PanicPoints into
    // `grid` in place, avoiding both spawn margins (see
    // BoardLayout::spawnColumnWidth). `p1Heart`/`p2Heart` are passed through
    // only because PanicPoint's own constructor needs them (see
    // PanicPoint::applyTileEffect) - this function has no opinion about what
    // a PanicPoint does with them. `rng` is the shared random source to
    // shuffle with (Board passes its own Board::rng()) when `useFixedPlacement`
    // is false - taken as a plain parameter rather than reaching for Board's
    // static accessor itself, so this stays fully standalone/testable
    // independent of Board. `useFixedPlacement` (PlayerVsRemote only - see
    // Board::generateSpecialTiles) skips the RNG/shuffle entirely and uses a
    // hardcoded coordinate list instead, so both peers always land on the
    // exact same board with no shared-seed synchronization to get wrong.
    void applySpecialTiles(
        std::map<std::pair<int, int>, std::unique_ptr<Tile>>& grid,
        const BoardLayout& layout,
        const std::function<sf::Vector2f(int, int)>& anchorToScreen,
        Heart* p1Heart, Heart* p2Heart,
        std::mt19937& rng, bool useFixedPlacement = false);
}
