#include "BoardGenerator.h"
#include "Tiles/LavaTile.h"
#include "Tiles/Hole.h"
#include "Tiles/PanicPoint.h"
#include <algorithm>
#include <vector>

namespace
{
    using Grid = std::map<std::pair<int, int>, std::unique_ptr<Tile>>;

    // PlayerVsRemote only: a hardcoded coordinate list guarantees both peers
    // get an identical board with no RNG left to desync. Valid only for
    // standardLayout() (7 rows, 20 cols, spawnColumnWidth 2).
    const std::vector<std::pair<int, int>> FIXED_SPECIAL_TILE_COORDS = {
        { 4, 0 }, { 13, 5 },   // lava
        { 8, 2 }, { 12, 4 },   // hole
        { 7, 3 },              // panic point
    };

    // Builds TileType at coords's screen position and inserts it into grid.
    // Args... forwards whatever extra constructor arguments TileType needs.
    template <typename TileType, typename... Args>
    void placeSpecialTile(Grid& grid, const std::pair<int, int>& coords,
        const std::function<sf::Vector2f(int, int)>& anchorToScreen, Args&&... extraArgs)
    {
        auto [q, row] = coords;
        grid[{q, row}] = std::make_unique<TileType>(q, row, anchorToScreen(q, row), std::forward<Args>(extraArgs)...);
    }
}

namespace BoardGenerator
{
    BoardLayout standardLayout()
    {
        return BoardLayout{
            /* rows */ 7,
            /* cols */ 20,
            /* lavaTileCount */ 2,
            /* holeTileCount */ 2,
            /* panicPointCount */ 1,
            /* spawnColumnWidth */ 2
        };
    }

    Grid buildBaseGrid(const BoardLayout& layout, const std::function<sf::Vector2f(int, int)>& anchorToScreen)
    {
        Grid grid;

        // Plain grid of ordinary tiles.
        for (int row = 0; row < layout.rows; ++row)
        {
            int start_col = (row % 2 == 0) ? 0 : 1;

            for (int q = start_col; q < layout.cols; q += 2)
            {
                grid[{q, row}] = std::make_unique<Tile>(q, row, anchorToScreen(q, row));
            }
        }

        return grid;
    }

    void applySpecialTiles(Grid& grid, const BoardLayout& layout,
        const std::function<sf::Vector2f(int, int)>& anchorToScreen,
        Heart* p1Heart, Heart* p2Heart, std::mt19937& rng, bool useFixedPlacement)
    {
        int needed = layout.lavaTileCount + layout.holeTileCount + layout.panicPointCount;
        std::vector<std::pair<int, int>> chosenCoords;

        if (useFixedPlacement)
        {
            if (static_cast<int>(FIXED_SPECIAL_TILE_COORDS.size()) < needed)
                return;
            chosenCoords.assign(FIXED_SPECIAL_TILE_COORDS.begin(), FIXED_SPECIAL_TILE_COORDS.begin() + needed);
        }
        else
        {
            // Only collect candidates outside the spawn margins.
            std::vector<std::pair<int, int>> allCoords;
            int minQ = layout.spawnColumnWidth;
            int maxQ = layout.cols - 1 - layout.spawnColumnWidth;
            for (auto const& [coords, tile] : grid)
            {
                if (coords.first >= minQ && coords.first <= maxQ)
                    allCoords.push_back(coords);
            }

            if (static_cast<int>(allCoords.size()) < needed)
                return;

            std::shuffle(allCoords.begin(), allCoords.end(), rng);
            chosenCoords.assign(allCoords.begin(), allCoords.begin() + needed);
        }

        int next = 0;
        for (int i = 0; i < layout.lavaTileCount; ++i, ++next)
            placeSpecialTile<LavaTile>(grid, chosenCoords[next], anchorToScreen);

        for (int i = 0; i < layout.holeTileCount; ++i, ++next)
            placeSpecialTile<Hole>(grid, chosenCoords[next], anchorToScreen);

        for (int i = 0; i < layout.panicPointCount; ++i, ++next)
            placeSpecialTile<PanicPoint>(grid, chosenCoords[next], anchorToScreen, p1Heart, p2Heart);
    }
}
