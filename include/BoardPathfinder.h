#pragma once
#include "Tiles/Tile.h"
#include <vector>
#include <map>
#include <utility>
#include <memory>

class Monster; // Forward declaration - Tile.h already pulls in the full type, but this documents the actual dependency directly.

// Owns Board's reachability/pathfinding BFS - "given a monster, which tiles
// can it move to or attack, and what's the tile-by-tile path to a specific
// one." Extracted out of Board so Board itself doesn't also have to BE a
// graph-search algorithm on top of everything else it does.
//
// Only ever constructed and owned by Board (see Board::m_pathfinder) - not
// part of Board's own public API. Board keeps its existing public
// getReachableTiles/getExtendedAttackOnlyTiles/getPathTo methods and simply
// forwards to this internally, so no other caller (GameplayState, AIPlayer,
// Board's own performMove/highlightNeighbors) needs to know this class
// exists or change how it talks to Board at all.
//
// Reads Board's own tile grid (given by reference at construction - the
// exact same grid object Board owns and populates/mutates over its
// lifetime, never copied) but never modifies it, and knows nothing about
// concrete Monster or Tile subtypes - only the Monster/Tile base-class
// interface (getRange/getAttackRange/isPassableFor/isOccupiedByEnemy/
// hasEntity), so it stays exactly as polymorphic as the code it was moved
// from.
class BoardPathfinder
{
public:
    explicit BoardPathfinder(const std::map<std::pair<int, int>, std::unique_ptr<Tile>>& grid);

    // שכבה 1: לוגיקה טהורה - "אילו tiles המפלצת יכולה להגיע/לתקוף אליהם", בלי לצייר כלום.
    std::vector<Tile*> getReachableTiles(Monster* monster) const;

    // Enemy tiles reachable ONLY because of a monster's extended attack
    // range (Monster::getAttackRange() > getRange()) - i.e. beyond normal
    // move/attack reach but still within the extended reach. Empty for
    // every monster whose getAttackRange() == getRange() (the default).
    std::vector<Tile*> getExtendedAttackOnlyTiles(Monster* monster) const;

    // שלב ב': אותה שאילתה, אבל מחזירה את המסלול המדורג (לפי סדר) מהמפלצת ל-target
    // הספציפי, לא רק "מה אפשר". target חייב להיות tile שכבר יצא מ-getReachableTiles
    // (כלומר תנועה, לא תקיפה) - אחרת מוחזרת רשימה ריקה.
    std::vector<Tile*> getPathTo(Monster* monster, Tile* target) const;

private:
    // ה-BFS המשותף (מעבר יחיד) שגם getReachableTiles וגם getPathTo נשענים עליו.
    void computeReachability(Monster* monster,
        std::vector<Tile*>& outReachable,
        std::map<std::pair<int, int>, std::pair<int, int>>& outParent,
        std::vector<Tile*>* outExtendedAttackOnly = nullptr) const;

    // The one place that answers "can this entity enter/traverse this
    // neighboring Tile" for the BFS above.
    bool visitNeighbor(Monster* monster, Tile* tile,
        const std::pair<int, int>& neighbor, const std::pair<int, int>& parent,
        int neighborDist, int range, int attackRange,
        std::vector<Tile*>& outReachable,
        std::map<std::pair<int, int>, std::pair<int, int>>& outParent,
        std::vector<Tile*>* outExtendedAttackOnly) const;

    // Records `tile` into outReachable (+ outParent) when within `range`, or
    // into outExtendedAttackOnly when beyond `range` but still within
    // `attackRange`.
    static void recordReachability(Tile* tile,
        const std::pair<int, int>& neighbor, const std::pair<int, int>& parent,
        int neighborDist, int range, int attackRange,
        std::vector<Tile*>& outReachable,
        std::map<std::pair<int, int>, std::pair<int, int>>& outParent,
        std::vector<Tile*>* outExtendedAttackOnly);

    const std::map<std::pair<int, int>, std::unique_ptr<Tile>>& m_grid;
};
