#include "BoardGenerator.h"
#include "Tiles/LavaTile.h"
#include "Tiles/Hole.h"
#include "Tiles/PanicPoint.h"
#include <algorithm>
#include <vector>
#include <iostream>

namespace
{
    using Grid = std::map<std::pair<int, int>, std::unique_ptr<Tile>>;

    // Shared by every special-tile placement below: builds TileType at
    // `coords`'s screen position and inserts it into `grid`. TileType is
    // always known at the call site (never chosen at runtime), and
    // constructor argument lists genuinely differ between Tile subtypes
    // (PanicPoint needs both Hearts; Hole/LavaTile need neither) - Args...
    // forwards whatever extra constructor arguments TileType needs beyond
    // (q, row, position).
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

        // שלב א': יצירת הלוח כרגיל עם משבצות רגילות
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
        Heart* p1Heart, Heart* p2Heart, std::mt19937& rng)
    {
        // אוספים משבצות רק מהאיזור המותר (בלי הטורים הקיצוניים של הזימונים והלבבות)
        std::vector<std::pair<int, int>> allCoords;
        int minQ = layout.spawnColumnWidth;
        int maxQ = layout.cols - 1 - layout.spawnColumnWidth;
        for (auto const& [coords, tile] : grid)
        {
            if (coords.first >= minQ && coords.first <= maxQ)
                allCoords.push_back(coords);
        }

        int needed = layout.lavaTileCount + layout.holeTileCount + layout.panicPointCount;
        if (static_cast<int>(allCoords.size()) < needed)
            return;

        std::shuffle(allCoords.begin(), allCoords.end(), rng);

        // TEMP DEBUG (see conversation) - remove once the desync is found.
        std::cout << "[BoardGenerator] allCoords.size()=" << allCoords.size() << " chosen coords:";

        int next = 0;
        for (int i = 0; i < layout.lavaTileCount; ++i, ++next)
        {
            std::cout << " Lava(" << allCoords[next].first << "," << allCoords[next].second << ")";
            placeSpecialTile<LavaTile>(grid, allCoords[next], anchorToScreen);
        }

        for (int i = 0; i < layout.holeTileCount; ++i, ++next)
        {
            std::cout << " Hole(" << allCoords[next].first << "," << allCoords[next].second << ")";
            placeSpecialTile<Hole>(grid, allCoords[next], anchorToScreen);
        }

        for (int i = 0; i < layout.panicPointCount; ++i, ++next)
        {
            std::cout << " Panic(" << allCoords[next].first << "," << allCoords[next].second << ")";
            placeSpecialTile<PanicPoint>(grid, allCoords[next], anchorToScreen, p1Heart, p2Heart);
        }
        std::cout << std::endl;
    }
}
