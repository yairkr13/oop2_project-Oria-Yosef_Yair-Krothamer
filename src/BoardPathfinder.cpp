#include "BoardPathfinder.h"
#include "HexGrid.h"
#include "Monsters/Monster.h"
#include <algorithm>

BoardPathfinder::BoardPathfinder(const std::map<std::pair<int, int>, std::unique_ptr<Tile>>& grid)
    : m_grid(grid)
{
}

void BoardPathfinder::recordReachability(Tile* tile,
    const std::pair<int, int>& neighbor, const std::pair<int, int>& parent,
    int neighborDist, int range, int attackRange,
    std::vector<Tile*>& outReachable,
    std::map<std::pair<int, int>, std::pair<int, int>>& outParent,
    std::vector<Tile*>* outExtendedAttackOnly)
{
    if (neighborDist <= range)
    {
        outReachable.push_back(tile);
        outParent[neighbor] = parent; // an entry here means movement can legally end here (see getPathTo)
    }
    else if (outExtendedAttackOnly && neighborDist <= attackRange)
    {
        // Beyond normal range but within extended attack range - attackable,
        // but never given an outParent entry (never path-able-to).
        outExtendedAttackOnly->push_back(tile);
    }
}

bool BoardPathfinder::visitNeighbor(const BoardEntity* entity, Tile* tile,
    const std::pair<int, int>& neighbor, const std::pair<int, int>& parent,
    int neighborDist, int range, int attackRange, bool includeAllies,
    std::vector<Tile*>& outReachable,
    std::map<std::pair<int, int>, std::pair<int, int>>& outParent,
    std::vector<Tile*>* outExtendedAttackOnly) const
{
    // The tile itself decides polymorphically whether the entity may pass through it.
    if (!tile->isPassableFor(entity))
    {
        // Impassable (including just "occupied") still counts as attackable
        // if it holds an enemy, or an ally when includeAllies was requested.
        if (tile->isOccupiedByEnemy(entity->getSide()) ||
            (includeAllies && tile->isOccupiedByAlly(entity->getSide())))
            recordReachability(tile, neighbor, parent, neighborDist, range, attackRange,
                outReachable, outParent, outExtendedAttackOnly);

        return false; // can't continue moving through this tile
    }

    recordReachability(tile, neighbor, parent, neighborDist, range, attackRange,
        outReachable, outParent, outExtendedAttackOnly);
    return true;
}

void BoardPathfinder::computeReachability(const BoardEntity* entity,
    std::vector<Tile*>& outReachable,
    std::map<std::pair<int, int>, std::pair<int, int>>& outParent,
    bool includeAllies,
    std::vector<Tile*>* outExtendedAttackOnly) const
{
    outReachable.clear();
    outParent.clear();
    if (outExtendedAttackOnly) outExtendedAttackOnly->clear();
    if (!entity) return;

    int q = entity->getQ();
    int row = entity->getRow();
    int range = entity->getRange();
    int attackRange = entity->getAttackRange(); // == range for every monster currently in the game
    int bfsLimit = std::max(range, attackRange); // walk far enough to find extended-only enemies too

    std::map<std::pair<int, int>, int> visited; // distance only - internal to this BFS
    std::vector<std::pair<int, int>> frontier;
    visited[{q, row}] = 0;
    frontier.push_back({ q, row });

    while (!frontier.empty())
    {
        std::vector<std::pair<int, int>> nextFrontier;
        for (auto [cq, cr] : frontier)
        {
            int dist = visited[{cq, cr}];
            if (dist >= bfsLimit)
                continue;

            for (auto const& offset : HexGrid::kNeighborOffsets)
            {
                std::pair<int, int> neighbor = { cq + offset.dq, cr + offset.dr };
                if (visited.count(neighbor))
                    continue;
                auto it = m_grid.find(neighbor);
                if (it == m_grid.end())
                    continue;

                Tile* tile = it->second.get();
                int neighborDist = dist + 1;

                bool canContinueThrough = visitNeighbor(entity, tile, neighbor, { cq, cr },
                    neighborDist, range, attackRange, includeAllies,
                    outReachable, outParent, outExtendedAttackOnly);

                if (!canContinueThrough)
                    continue;

                visited[neighbor] = neighborDist; // keep walking through this tile even beyond range,
                // to find an extended-range enemy farther out

                if (!tile->hasEntity())
                    nextFrontier.push_back(neighbor);
            }
        }
        frontier = std::move(nextFrontier);
    }
}

std::vector<const Tile*> BoardPathfinder::getReachableTiles(const BoardEntity* entity, bool includeAllies) const
{
    std::vector<Tile*> reachable;
    std::map<std::pair<int, int>, std::pair<int, int>> parent; // unused here, just required by computeReachability
    computeReachability(entity, reachable, parent, includeAllies);
    return { reachable.begin(), reachable.end() };
}

// Walks backward from target through outParent to the source, then reverses.
// No outParent entry means unreachable, so an empty path is returned.
std::vector<const Tile*> BoardPathfinder::getPathTo(const BoardEntity* entity, const Tile* target) const
{
    std::vector<Tile*> path;
    // Nothing to path for without both a mover and a destination.
    if (!entity || !target) return { path.begin(), path.end() };

    std::vector<Tile*> reachable;
    std::map<std::pair<int, int>, std::pair<int, int>> parent;
    computeReachability(entity, reachable, parent);

    std::pair<int, int> sourceCoords = { entity->getQ(), entity->getRow() };
    std::pair<int, int> targetCoords = { target->getQ(), target->getRow() };

    // Already standing there (no path needed), or target never showed up in
    // outParent at all - i.e. genuinely unreachable. Either way: empty path.
    if (targetCoords == sourceCoords) return { path.begin(), path.end() };
    if (!parent.count(targetCoords)) return { path.begin(), path.end() };

    std::vector<std::pair<int, int>> reversedCoords;
    std::pair<int, int> cur = targetCoords;
    while (cur != sourceCoords)
    {
        reversedCoords.push_back(cur);
        cur = parent.at(cur);
    }
    std::reverse(reversedCoords.begin(), reversedCoords.end());

    for (auto& coords : reversedCoords)
    {
        auto it = m_grid.find(coords);
        if (it != m_grid.end())
            path.push_back(it->second.get());
    }

    return { path.begin(), path.end() };
}
