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
        outParent[neighbor] = parent; // an entry here means "movement can
        // legally end here" (see getPathTo) - an extended-only tile below
        // must never get one.
    }
    else if (outExtendedAttackOnly && neighborDist <= attackRange)
    {
        // Beyond normal range, still within the extended attack range -
        // attackable, but deliberately never given an outParent entry:
        // never path-able-to, never movement-legal.
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
    // בדיקת עבירות פולימורפית: המשבצת מחליטה בעצמה אם הישות יכולה לעבור
    if (!tile->isPassableFor(entity))
    {
        // אם המשבצת לא עבירה לתנועה (כולל בגלל שהיא תפוסה - ראו Tile::setEntity),
        // עדיין נבדוק אם יש עליה אויב שניתן לתקוף מרחוק/באוויר - ואם includeAllies
        // ביקש זאת, גם אם יש עליה בן-ברית (למטרות Special, לא תנועה/תקיפה).
        // Impassable but still a valid attack/Special target: an enemy is
        // always recorded, an ally only when includeAllies was requested.
        if (tile->isOccupiedByEnemy(entity->getSide()) ||
            (includeAllies && tile->isOccupiedByAlly(entity->getSide())))
            recordReachability(tile, neighbor, parent, neighborDist, range, attackRange,
                outReachable, outParent, outExtendedAttackOnly);

        return false; // הישות לא יכולה להמשיך לנוע דרך המשבצת הזו
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
    int attackRange = entity->getAttackRange(); // == range for every monster currently in the game (no monster overrides getAttackRange any more)
    int bfsLimit = std::max(range, attackRange); // walk far enough to find extended-only enemies too

    std::map<std::pair<int, int>, int> visited; // מרחק בלבד - פנימי לחישוב, לא מוחזר
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

                // The one place "can this entity enter/traverse this
                // neighboring tile" is decided - see visitNeighbor above.
                bool canContinueThrough = visitNeighbor(entity, tile, neighbor, { cq, cr },
                    neighborDist, range, attackRange, includeAllies,
                    outReachable, outParent, outExtendedAttackOnly);

                if (!canContinueThrough)
                    continue;

                visited[neighbor] = neighborDist; // needed so the BFS keeps
                // walking through this tile (e.g. to find an extended-range
                // enemy farther out) even when it's beyond `range` itself.

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
    std::map<std::pair<int, int>, std::pair<int, int>> parent; // לא בשימוש כאן, רק כי computeReachability דורש אותו
    computeReachability(entity, reachable, parent, includeAllies);
    return { reachable.begin(), reachable.end() };//to be a const
}

// שחזור המסלול: הולכים אחורה מה-target דרך outParent עד שמגיעים למקור, ואז
// הופכים את הסדר (כי בנינו אותו מהסוף להתחלה). אם target לא הופיע ב-parent
// בכלל - זה אומר שהוא לא נגיש (או שהוא עצמו נקודת המוצא), ומוחזרת רשימה ריקה.
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
