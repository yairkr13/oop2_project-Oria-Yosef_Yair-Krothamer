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
    int lavaTileCount;
    int holeTileCount;
    int panicPointCount; // expected to be 0 or 1 - kept as a count rather
                          // than a bool so a future map could ask for more
                          // without this struct needing to change again.

    // How many columns on each side are reserved for spawning and therefore
    // never eligible for a special tile - see Board::getSpawnableTiles/
    // highlightSpawnTiles, which independently hardcode this same "2
    // columns" assumption today. Kept here (rather than hardcoded inside
    // BoardGenerator) so at least tile-placement respects a per-layout
    // value; those two Board methods are unchanged by this refactor and
    // would need their own small follow-up to become fully layout-size-
    // aware too - see the accompanying write-up.
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

    // Randomly swaps `layout.lavaTileCount` LavaTiles, `layout.holeTileCount`
    // Holes, and (if `layout.panicPointCount > 0`) that many PanicPoints into
    // `grid` in place, avoiding both spawn margins (see
    // BoardLayout::spawnColumnWidth). `p1Heart`/`p2Heart` are passed through
    // only because PanicPoint's own constructor needs them (see
    // PanicPoint::applyTileEffect) - this function has no opinion about what
    // a PanicPoint does with them. `rng` is the shared random source to
    // shuffle with (Board passes its own Board::rng()) - taken as a plain
    // parameter rather than reaching for Board's static accessor itself, so
    // this stays fully standalone/testable independent of Board.
    void applySpecialTiles(
        std::map<std::pair<int, int>, std::unique_ptr<Tile>>& grid,
        const BoardLayout& layout,
        const std::function<sf::Vector2f(int, int)>& anchorToScreen,
        Heart* p1Heart, Heart* p2Heart,
        std::mt19937& rng);
}
