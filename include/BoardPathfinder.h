#pragma once
#include "Tiles/Tile.h"
#include <vector>
#include <map>
#include <utility>
#include <memory>

class Monster; // Forward declaration - Tile.h already pulls in the full type, but this documents the actual dependency directly.

// Owns Board's reachability/pathfinding BFS: given an entity, which tiles
// can it move to or attack, and what's the tile-by-tile path to one of them.
// Extracted out of Board so Board isn't also a graph-search algorithm.
// Only ever constructed and owned by Board (m_pathfinder); reads Board's
// grid by reference but never modifies it.
class BoardPathfinder
{
public:
    explicit BoardPathfinder(const std::map<std::pair<int, int>, std::unique_ptr<Tile>>& grid);

    // includeAllies (default false) also records ally-occupied tiles that
    // are otherwise impassable purely because they're occupied - needed for
    // Special-ability target search.
    std::vector<const Tile*> getReachableTiles(const BoardEntity* entity, bool includeAllies = false) const;

    // Enemy tiles reachable ONLY via an extended attack range
    // (getAttackRange() > getRange()) - currently always empty since no
    // monster's attack range differs from its move range any more. Kept
    // commented (not deleted): the underlying plumbing stays in
    // computeReachability either way, shared with the two methods above/below.
    //std::vector<Tile*> getExtendedAttackOnlyTiles(Monster* monster) const;
    //std::vector<const Tile*> getExtendedAttackOnlyTiles(const BoardEntity* entity) const;

    // Same reachability, but returns the ordered tile-by-tile path to
    // target. target must be a tile already returned by getReachableTiles
    // (movement, not attack) or an empty list is returned.
    std::vector<const Tile*> getPathTo(const BoardEntity* entity, const Tile* target) const;
private:
    // The single-pass BFS both getReachableTiles and getPathTo build on.
    void computeReachability(const BoardEntity* entity,
        std::vector<Tile*>& outReachable,
        std::map<std::pair<int, int>, std::pair<int, int>>& outParent,
        bool includeAllies = false,
        std::vector<Tile*>* outExtendedAttackOnly = nullptr) const;

    // Decides whether this entity can enter/traverse one neighboring Tile during the BFS.
    bool visitNeighbor(const BoardEntity* entity, Tile* tile,
        const std::pair<int, int>& neighbor, const std::pair<int, int>& parent,
        int neighborDist, int range, int attackRange, bool includeAllies,
        std::vector<Tile*>& outReachable,
        std::map<std::pair<int, int>, std::pair<int, int>>& outParent,
        std::vector<Tile*>* outExtendedAttackOnly) const;

    // Records tile into outReachable (+ outParent) when within range, or
    // into outExtendedAttackOnly when beyond range but still within attackRange.
    static void recordReachability(Tile* tile,
        const std::pair<int, int>& neighbor, const std::pair<int, int>& parent,
        int neighborDist, int range, int attackRange,
        std::vector<Tile*>& outReachable,
        std::map<std::pair<int, int>, std::pair<int, int>>& outParent,
        std::vector<Tile*>* outExtendedAttackOnly);

    const std::map<std::pair<int, int>, std::unique_ptr<Tile>>& m_grid;
};
