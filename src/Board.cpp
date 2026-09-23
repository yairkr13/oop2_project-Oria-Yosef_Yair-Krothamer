#include "Board.h"
#include "Attacks/AttackAnimation.h"
#include <iostream>
#include <utility>
#include "Constants.h"
#include "SoundPlayer.h"
#include <stdexcept>
#include <string>

namespace
{
    // The smallest board this game supports - a floor, not derived from
    // BoardGenerator::standardLayout() itself (checking a layout against
    // its own source would be circular). A smaller board was never the one
    // actually tuned/played, so this stops one from ever being constructed
    // rather than silently accepting something untested.
    constexpr int MIN_ROWS = 7;
    constexpr int MIN_COLS = 14;

    // A board's total on-screen footprint, in pixels, for `count` tiles
    // along one axis, at the current Config::TILE_RADIUS - the same
    // hexWidth/1.5*TILE_RADIUS step tileAnchor() itself uses, plus one full
    // tile (2*TILE_RADIUS) so the footprint covers the first and last
    // tile's own far edge, not just their anchors. Shared by both axes
    // (`step` is hexWidth/2 for columns, 1.5*TILE_RADIUS for rows) so
    // Board::Board's centering math and its own validation agree on exactly
    // what "the board's size" means.
    float boardExtent(int count, float step)
    {
        return step * static_cast<float>(count - 1) + 2.f * Config::TILE_RADIUS;
    }
}

Board::Board(const BoardLayout& layout)
    : m_layout(layout), m_pathfinder(m_grid)
{
    // Zero/negative rows or cols would make the loops below (createBoard,
    // etc.) either build nothing or misbehave - reject it outright instead.
    if (m_layout.rows <= 0 || m_layout.cols <= 0)
    {
        throw std::invalid_argument(
            "BoardLayout rows/cols must be positive (got rows=" + std::to_string(m_layout.rows) +
            ", cols=" + std::to_string(m_layout.cols) + ")");
    }

    // Below the smallest board this game was ever tuned/played on - reject
    // it rather than silently accepting an untested shape (see MIN_ROWS/
    // MIN_COLS above).
    if (m_layout.rows < MIN_ROWS || m_layout.cols < MIN_COLS)
    {
        throw std::invalid_argument(
            "BoardLayout " + std::to_string(m_layout.rows) + "x" + std::to_string(m_layout.cols) +
            " is smaller than the minimum supported " + std::to_string(MIN_ROWS) + "x" + std::to_string(MIN_COLS));
    }

    // Left's spawn band is columns [0, spawnColumnWidth-1], Right's is
    // [cols-spawnColumnWidth, cols-1] (see Board::spawnColumnRange) - too
    // few columns makes those two bands overlap (or coincide entirely),
    // letting both sides spawn onto the same tiles. Needs cols to fit both
    // bands with no shared column.
    if (m_layout.cols < 2 * m_layout.spawnColumnWidth)
    {
        throw std::invalid_argument(
            "BoardLayout cols=" + std::to_string(m_layout.cols) + " is too small for spawnColumnWidth=" +
            std::to_string(m_layout.spawnColumnWidth) + " (needs cols >= " +
            std::to_string(2 * m_layout.spawnColumnWidth) + ", or the two sides' spawn columns overlap)");
    }

    float hexWidth = std::sqrt(3.f) * Config::TILE_RADIUS;
    float boardWidth = boardExtent(m_layout.cols, hexWidth / 2.f);
    float boardHeight = boardExtent(m_layout.rows, 1.5f * Config::TILE_RADIUS);

    // The board only ever needs to fit above the bottom panel, not the
    // window's full height - the panel is reserved space regardless of
    // board shape (see GameplayState's m_bottomPanel).
    float playAreaHeight = static_cast<float>(Config::WINDOW_HEIGHT) - Config::BOTTOM_PANEL_HEIGHT;

    // The computed pixel footprint must fit inside the window (width) and
    // above the bottom panel (height) - otherwise this layout can't be
    // drawn at the current Config::TILE_RADIUS.
    if (boardWidth > static_cast<float>(Config::WINDOW_WIDTH) || boardHeight > playAreaHeight)
    {
        throw std::out_of_range(
            "BoardLayout " + std::to_string(m_layout.rows) + "x" + std::to_string(m_layout.cols) +
            " is too large to fit the window (needs " + std::to_string(static_cast<int>(boardWidth)) + "x" +
            std::to_string(static_cast<int>(boardHeight)) + "px, only " +
            std::to_string(Config::WINDOW_WIDTH) + "x" + std::to_string(static_cast<int>(playAreaHeight)) +
            "px available above the bottom panel)");
    }

    // Centered - horizontally in the window, vertically in the play area
    // above the bottom panel - instead of anchored at a fixed point, so a
    // wider/taller BoardLayout stays centered instead of growing off to one
    // side (see the m_startX/m_startY comment in Board.h).
    m_startX = (static_cast<float>(Config::WINDOW_WIDTH) - boardWidth) / 2.f;
    m_startY = (playAreaHeight - boardHeight) / 2.f;

    createBoard();
}

void Board::createBoard()//למה לא קוראים לINIT HEART ולמשבצות המיוחדות מפה??????
{
    // BoardGenerator owns the actual tile-construction knowledge (grid
    // shape, and - later, in generateSpecialTiles - which concrete Tile
    // subtypes exist) - Board just decides *when* this happens (here, at
    // construction) and hands in its own tileAnchor() so the generator
    // never needs to know Board's pixel constants either.
    m_grid = BoardGenerator::buildBaseGrid(m_layout,
        [this](int q, int row) { return tileAnchor(q, row); });
}

void Board::draw(sf::RenderWindow& window, PlayerSide currentSide) const
{
    // Two full passes, deliberately NOT interleaved tile-by-tile (hex,
    // occupant, hex, occupant, ...): m_grid is a std::map keyed by
    // (q, row), so iterating it draws tiles left-to-right (sorted by q).
    // A moving entity is drawn as part of whichever tile it's LOGICALLY
    // standing on (its target tile - see Board::performMove, which
    // reassigns it there before the walk animation even starts), which is
    // not necessarily the tile under its current on-screen/animated
    // position (Monster::m_screenPos, interpolated in Monster::update).
    //
    // Interleaving meant a tile with a greater q than the moving entity's
    // target tile - i.e. a tile still ahead of it in iteration order -
    // got its semi-transparent hex painted on top of that entity's sprite
    // whenever the entity's animated position still overlapped it. Moving
    // right-to-left, the target tile's q is SMALLER than the source's, so
    // every tile in between (q between target and source) is drawn AFTER
    // the entity and stamps its hex on top of it for the whole animation -
    // exactly the "gray/darker, looks stuck under a tile" glitch. Moving
    // left-to-right never showed it: the target tile's q is the larger
    // one, so it (and the entity drawn with it) is always painted last.
    //
    // Drawing every tile's hex first, then every tile's occupant, makes
    // every occupant draw strictly on top of every hex regardless of grid
    // iteration order, so this no longer depends on movement direction.

    // 1. ציור כל המשבצות עצמן
    //לא צריך לצייר אותם רק פעם אחת???
    for (auto const& [coords, tile] : m_grid)
    {
        tile->draw(window);
    }
    
    // 2. ציור כל הישויות שנמצאות על המשבצות - תמיד מעל כל המשבצות
    //למה השחקן לא עושה את זה ??????
    for (auto const& [coords, tile] : m_grid)
    {
        tile->drawEntity(window, currentSide);
    }
}

void Board::initPlayerHearts(Heart* p1Heart, Heart* p2Heart, bool useFixedSpecialTiles)
{
    int middleRow = getMiddleRow(); // was hardcoded to 3 - now follows m_layout.rows, whatever the layout

    if (p1Heart != nullptr)
        spawnEntityOnTile(p1Heart, getExtremeTileInRow(middleRow, true));
    if (p2Heart != nullptr)
        spawnEntityOnTile(p2Heart, getExtremeTileInRow(middleRow, false));

    generateSpecialTiles(p1Heart, p2Heart, useFixedSpecialTiles);
}

void Board::generateSpecialTiles(Heart* p1Heart, Heart* p2Heart, bool useFixedPlacement)
{
    BoardGenerator::applySpecialTiles(m_grid, m_layout,
        [this](int q, int row) { return tileAnchor(q, row); },
        p1Heart, p2Heart, rng(), useFixedPlacement);
}

std::pair<int, int> Board::spawnColumnRange(PlayerSide side) const
{
    return (side == PlayerSide::Left)
        ? std::pair{ 0, m_layout.spawnColumnWidth - 1 }
        : std::pair{ m_layout.cols - m_layout.spawnColumnWidth, m_layout.cols - 1 };
}

void Board::highlightSpawnTiles(PlayerSide side) const
{
    clearHighlights();

    auto [minQ, maxQ] = spawnColumnRange(side);

    std::vector<const Tile*> spawnTiles;
    for (auto& [coords, tile] : m_grid)
    {
        if (tile->hasEntity()) continue;

        // Only tiles within this side's own spawn column band count.
        if (coords.first >= minQ && coords.first <= maxQ)
        {
            spawnTiles.push_back(tile.get());
        }
    }
    highlightTiles(spawnTiles); // default green, same as before
}

// Board stays the public facade for all three of these (see Board.h) - the
// actual BFS now lives entirely in BoardPathfinder (see m_pathfinder and
// BoardPathfinder.h/.cpp). Each is now a one-line forward; no caller of
// Board needed to change.
std::vector<const Tile*> Board::getReachableTiles(const BoardEntity* entity, bool includeAllies) const
{
    return m_pathfinder.getReachableTiles(entity, includeAllies);
}

std::vector<const Tile*> Board::getReachableOccupiedTiles(const BoardEntity* entity, bool includeAllies) const
{
    std::vector<const Tile*> occupied;
    for (const Tile* tile : getReachableTiles(entity, includeAllies))
        if (tile->hasEntity())
            occupied.push_back(tile);
    return occupied;
}

std::vector<const Tile*> Board::getPathTo(const BoardEntity* entity, const Tile* target) const
{
    return m_pathfinder.getPathTo(entity, target);
}

// שכבה 2: wrapper דק - קורא לשאילתה הטהורה למעלה, ורק אחראי על הצביעה (side effect
// ויזואלי). זו הפונקציה שהקליק האנושי קורא לה. AIPlayer, לעומת זאת, יוכל בעתיד לקרוא
// ישירות ל-getReachableTiles ולא לצייר כלום - הוא לא צריך צביעה, רק את הרשימה.
void Board::highlightNeighbors(const BoardEntity* entity) const //כן - ראו ההסבר ב-Board.h ליד getMutableTileAt
{
    if (!entity) return;

    // Split into the two groups highlightTiles paints separately below,
    // instead of each tile deciding-and-painting itself inline - same
    // reachable-tiles source as before, just routed through the shared
    // primitive now.
    std::vector<const Tile*> enemyTiles, moveTiles;
    for (const Tile* tile : getReachableTiles(entity))
        (tile->isOccupiedByEnemy(entity->getSide()) ? enemyTiles : moveTiles).push_back(tile);

    highlightTiles(moveTiles);                              // ירוק (ברירת המחדל) - ניתן לזוז
    highlightTiles(enemyTiles, sf::Color(255, 90, 90, 180)); // אדום - ניתן לתקוף
}

bool Board::selectEntity(const BoardEntity* entity, PlayerSide side) const
{
    if (!entity || !entity->canBeSelectedBy(side)) return false;

    clearHighlights();
    highlightNeighbors(entity); // מקבל BoardEntity* באופן טבעי!
    return true;
}

void Board::highlightTiles(const std::vector<const Tile*>& tiles, const sf::Color& color) const
{
    for (const Tile* constTile : tiles)
    {
        if (!constTile) continue;

        // constTile came from a read-only query (getReachableTiles, etc.) -
        // look up the real, mutable Tile* by coordinates, same pattern every
        // other caller already used before this was centralized here.
        Tile* tile = getMutableTileAt(constTile->getQ(), constTile->getRow());
        if (tile) tile->setHighlighted(true, color);
    }
}

void Board::clearHighlights() const
{
    for (auto& pair : m_grid)
    {
        pair.second->setHighlighted(false);
    }
}

sf::Vector2f Board::tileAnchor(int q, int row) const
{
    float width = std::sqrt(3.f) * Config::TILE_RADIUS;
    return { m_startX + (width / 2.f) * q, m_startY + (1.5f * Config::TILE_RADIUS) * row };
}

sf::Vector2f Board::tileToScreen(int q, int row) const
{
    sf::Vector2f anchor = tileAnchor(q, row);
    return { anchor.x + Config::TILE_RADIUS, anchor.y + Config::TILE_RADIUS };
}

void Board::updateTileEffects() const
{
    for (auto& [coords, tile] : m_grid)
    {
        tile->applyTileEffect(); // פולימורפיזם בפעולה! משבצת רגילה לא תעשה כלום, לבה תוריד חיים.

        // אם המפלצת מתה מהאפקט (למשל מהלבה), ננקה אותה מהמשבצת
		//no need this anymore because the entity has the tile!!!!!!!!
        // Generic per-turn-boundary tick (see BoardEntity::onTurnBoundary) -
        // this is the one existing place a "a player switch just happened"
        // event already reaches every occupied tile, so turn-scoped status
        // effects (Henrietta's Protection, Barzilla's empowered attack)
        // piggyback on it instead of a new timer. Board never learns what
        // any status effect means - it doesn't even need getEntity() any
        // more for this, tile->tickTurnBoundary() handles its own entity
        // (including the isReadyForRemoval()/clearEntity() cleanup) itself.
        tile->tickTurnBoundary();
    }
}

const Tile* Board::getExtremeTileInRow(int row, bool findLeftmost) const {
    Tile* bestTile = nullptr;
    int bestQ = findLeftmost ? std::numeric_limits<int>::max() : std::numeric_limits<int>::min();

    for (const auto& [coords, tile] : m_grid) {
        if (coords.second == row) {
            // Keep whichever candidate is farther in the requested direction
            // (smallest q for leftmost, largest q for rightmost).
            if ((findLeftmost && coords.first < bestQ) || (!findLeftmost && coords.first > bestQ)) {
                bestQ = coords.first;
                bestTile = tile.get();
            }
        }
    }
    return bestTile;
}


// AI_FindBestTargetForMonster עברה ל-AIPlayer::findBestTarget - היא הייתה מכילה
// היוריסטיקה (תקיפה > תנועה שמאלה), לא שאילתה עובדתית, אז לא הייתה שייכת ל-Board.
// AIPlayer עכשיו קורא ל-getReachableTiles() (למעלה בקובץ) בעצמו במקומה.

// שאילתה עובדתית בלבד - "אילו tiles פנויים בשטח הזימון של הצד הזה?" Board לא
// בוחר אף אחת מהן - זו הייתה בדיוק העבודה של AI_SpawnMonster הישנה, שגם אספה
// מועמדים וגם הגרילה אחד מהם. עכשיו רק החלק הראשון (איסוף) נשאר כאן.
std::vector<const Tile*> Board::getSpawnableTiles(const Monster* monster, PlayerSide side) const
{
    std::vector<const Tile*> spawnable;
    if (!monster || monster->isOnBoard()) return spawnable;

    auto [minQ, maxQ] = spawnColumnRange(side);

    for (auto& [coords, tile] : m_grid)
    {
        // Only tiles within this side's own spawn column band count.
        if (coords.first >= minQ && coords.first <= maxQ)
        {
            // ...and only if actually free and passable for this monster.
            if (!tile->hasEntity() && tile->isPassableFor(monster))
            {
                spawnable.push_back(tile.get());
            }
        }
    }

    return spawnable;
}
bool Board::spawnEntityOnTile(BoardEntity* entity,const Tile* targetTile) const
{
    if (!entity || !targetTile) return false;

    // הלוח הממיר את ה-const Tile* שקיבל ל-Tile* הפנימי של הלוח
    Tile* internalTile = getMutableTileAt(targetTile->getQ(), targetTile->getRow());

    // Reject the spawn outright if the tile is missing, already occupied,
    // or not passable for this entity (e.g. lava for a non-flyer).
    if (!internalTile || internalTile->hasEntity() || !internalTile->isPassableFor(entity))
        return false;

    internalTile->setEntity(entity);
    entity->spawnOnBoard(internalTile->getQ(), internalTile->getRow(), internalTile->getScreenPosition());

    SoundPlayer::getInstance().play("summon_sound");
    return true;
}

void Board::performAction(BoardEntity* entity,const Tile* constTargetTile) const
{
    if (!entity || !constTargetTile) return;
    Tile* targetTile = getMutableTileAt(constTargetTile->getQ(), constTargetTile->getRow());
    if (!targetTile) return;

    if (targetTile->isOccupiedByEnemy(entity->getSide()))
    {
        performAttack(entity, targetTile);
        return;
    }

    // Movement is Monster-only (Heart has no legal movement) - the one safe
    // downcast performMove needs happens here, once.
    if (entity->canMove())//לא אוהבת את זה
        performMove(entity, targetTile);
}

void Board::performAttack(BoardEntity* entity, Tile* targetTile) const
{
    // Give the attacker a chance to supply an animated attack (see
    // BoardEntity::createAttackAnimation). Most entities don't override
    // it, so this is nullptr and the attack resolves immediately below,
    // exactly as before. An entity that does provide one (e.g. Mozzy)
    // gets its damage deferred until the animation reports impact -
    // Board never needs to know which concrete entity/animation this is,
    // nor does it ever compute or inspect a damage value: that stays
    // entirely below Tile::receiveAttackFrom, inside Monster::attack().
    // Passes the target TILE's own screen position, not the target entity -
    // no createAttackAnimation override needs anything else from the
    // target, so Board no longer needs targetTile->getEntity() at all here.
    if (std::unique_ptr<AttackAnimation> animation = entity->createAttackAnimation(targetTile->getScreenPosition()))
    {
        animation->setOnImpact([targetTile, entity]() {
            targetTile->receiveAttackFrom(entity);
        });

        // Board's job ends at deciding the attack happens and wiring
        // how it eventually resolves - from here the attacker owns and
        // drives its own animation (update/draw/isAttacking), the same
        // ownership split it already has for its own movement.
        entity->playAttackAnimation(std::move(animation));
    }
    else
    {
        targetTile->receiveAttackFrom(entity);
    }
}

void Board::performMove(BoardEntity* entity, Tile* targetTile) const
{
    // Movement is only ever legal onto a tile within this monster's NORMAL
    // range - an extended attack-only range (see Monster::getAttackRange;
    // currently never overridden by any monster, but the check stays
    // correct either way) would let it strike farther, never walk farther.
    // Checked explicitly against getReachableTiles here rather
    // than relying on getPathTo coming back empty for such a tile, since
    // the no-path fallback a few lines below would otherwise still
    // teleport the monster there directly.
    std::vector<const Tile*> reachable = getReachableTiles(entity);
    bool isMoveLegal = !targetTile->hasEntity() && targetTile->isPassableFor(entity)
        && std::find(reachable.begin(), reachable.end(), targetTile) != reachable.end();

    if (!isMoveLegal) return;

    // היה: entity->getCurrentTile() (קשר דו-כיווני שהוסר - ראו BoardEntity.h).
    // עכשיו: המפלצת יודעת את המיקום שלה בעצמה (getQ/getRow, מקור אמת יחיד),
    // ו-Board מחפשת את ה-Tile המתאים ב-m_grid שלה.
    Tile* sourceTile = getMutableTileAt(entity->getQ(), entity->getRow());
    if (sourceTile == nullptr) return;

    // בונים את המסלול המדורג (משבצת-משבצת) במקום לקפוץ בקו ישר ליעד
    std::vector<const Tile*> path = getPathTo(entity, targetTile);
    std::vector<sf::Vector2f> pathScreenPositions;
    pathScreenPositions.reserve(path.size());
    for (const Tile* step : path)
        pathScreenPositions.push_back(step->getScreenPosition());

    // רשת ביטחון: אם משום מה לא נמצא מסלול (לא אמור לקרות, כי targetTile
    // כבר אושר כנגיש), נופלים חזרה על תזוזה ישירה כדי שהמפלצת לא "תיתקע"
    if (pathScreenPositions.empty())
        pathScreenPositions.push_back(targetTile->getScreenPosition());

    targetTile->setEntity(entity);
    sourceTile->clearEntity();

    entity->moveAlongPath(targetTile->getQ(), targetTile->getRow(), pathScreenPositions);
}

void Board::update(float dt) const
{
    for (auto& [coords, tile] : m_grid)
    {
		//change this to one function!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        // Advances this tile's own entity's animation state, whatever it may
        // be - movement (Monster::m_pathQueue) and an in-flight attack
        // animation (Monster::m_attackAnimation) are both driven from the
        // entity's own update() override. Board just relays the per-frame
        // tick; it owns none of that state itself, and no longer needs
        // getEntity() to relay it - tile->updateEntity(dt) is a no-op on an
        // empty tile, and already handles the isReadyForRemoval()/
        // clearEntity() cleanup for a just-finished death animation itself.
        tile->updateEntity(dt);
    }
}

bool Board::isAnimating() const
{
    // "Is the board busy" is answered purely by asking each occupied tile's
    // entity whether *it* is animating - Board aggregates, it doesn't own
    // any animation itself and doesn't know or care why an entity says yes
    // (movement, an attack animation, or anything added later). Every
    // existing gate (TurnManager, GameplayState, AIPlayer) already calls
    // only this method, so none of them need to change either.
    for (auto const& [coords, tile] : m_grid)
    {
        if (tile->isEntityAnimating()) return true;
    }
    return false; // <--- חסר לך את זה! אם אף אחד לא זז, מחזירים שקר
}

std::pair<int, int> Board::screenToTile(const sf::Vector2f& pos) const
{
    // מזיזים את pos למערכת הצירים שעליה tileToScreen בנוי - מחסירים את נקודת
    // המוצא (m_startX/m_startY) וגם את TILE_RADIUS ש-tileToScreen מוסיף בסוף כדי
    // למרכז את המשושה (ה-shape עצמו ממוקם לפי הפינה השמאלית-עליונה, לא המרכז).
    float x = pos.x - m_startX - Config::TILE_RADIUS;
    float y = pos.y - m_startY - Config::TILE_RADIUS;

    // 1. המרה ל-axial שברי (pointy-top hex, size = TILE_RADIUS)
    float q_axial_frac = (std::sqrt(3.f) / 3.f * x - 1.f / 3.f * y) / Config::TILE_RADIUS;
    float r_axial_frac = (2.f / 3.f * y) / Config::TILE_RADIUS;

    // 2. עיגול מדויק דרך cube coordinates
    float x_cube = q_axial_frac;
    float z_cube = r_axial_frac;
    float y_cube = -x_cube - z_cube;

    int rx = static_cast<int>(std::round(x_cube));
    int ry = static_cast<int>(std::round(y_cube));
    int rz = static_cast<int>(std::round(z_cube));

    float x_diff = std::abs(rx - x_cube);
    float y_diff = std::abs(ry - y_cube);
    float z_diff = std::abs(rz - z_cube);

    // Standard cube-rounding fixup: recompute whichever of the three rounded
    // coordinates had the largest error from its own component, so x+y+z
    // still sums to zero after rounding.
    if (x_diff > y_diff && x_diff > z_diff)
        rx = -ry - rz;
    else if (y_diff > z_diff)
        ry = -rx - rz;
    else
        rz = -rx - ry;

    int qAxial = rx;
    int row = rz; // rz == r_axial המעוגל, שהוא בדיוק ה-row שלנו

    // 3. axial -> doubled coordinates (מערכת ה-q שהפרויקט משתמש בה בפועל)
    int qDouble = 2 * qAxial + row;

    return { qDouble, row };
}

// Board.cpp - הוספה, לא נוגעת ב-Card בכלל, רק ב-Tile/geometry
const Tile* Board::pickRandomTile(const std::vector<const Tile*>& tiles) const
{
    if (tiles.empty()) return nullptr;

    std::uniform_int_distribution<size_t> dist(0, tiles.size() - 1);
    return tiles[dist(rng())];
}

const Tile* Board::getTileAt(int q, int row) const
{
    return getMutableTileAt(q, row);
}

const Tile* Board::getTileAtScreenPosition(const sf::Vector2f& pos) const
{
    auto [q, row] = screenToTile(pos);
    return getTileAt(q, row);
}

Tile* Board::getMutableTileAt(int q, int row) const
{
    auto it = m_grid.find({ q, row });
    return (it != m_grid.end()) ? it->second.get() : nullptr;
}

void Board::highlightValidSpecialTargets(const Monster* caster) const
{
    if (!caster) return;

    // Bounded by range now (getReachableTiles - the exact same reachability
    // query move/attack highlighting already uses) instead of checking every
    // occupied tile on the whole board. includeAllies=true is required here:
    // an occupied tile is normally excluded from getReachableTiles entirely
    // (see Tile::setEntity - occupancy alone makes it "impassable"), with
    // only an enemy getting the usual "impassable but still attackable"
    // exception. Ally-targeted Specials (Muffintop, Henrietta, Barzilla)
    // need that same exception extended to allies, or they'd never find a
    // valid target at all.
    std::vector<const Tile*> inRange = getReachableTiles(caster, /*includeAllies=*/true);

    // Whole range shown first, in the caster's own Special color but
    // lighter (halved alpha) - so the player sees the full reach, not only
    // the tiles that happen to hold a valid target right now. Reuses the
    // same per-monster color isValidSpecialTarget's highlight already comes
    // from (see getSpecialTargetHighlightColor) rather than a separate,
    // unrelated "range" color - so it's immediately obvious which range
    // belongs to which caster's Special.
    sf::Color rangeColor = caster->getSpecialTargetHighlightColor();
    rangeColor.a /= 2;
    highlightTiles(inRange, rangeColor);

    // Then the actual valid targets, at full color, on top - standing out
    // from the plain range around them. Filters the SAME inRange computed
    // above (one BFS call total) rather than calling getReachableOccupiedTiles,
    // which would run getReachableTiles a second time for no reason - this
    // function already has the full range in hand. getReachableOccupiedTiles
    // itself stays as the shared query for a caller (AIPlayer) that only
    // ever needs the occupied subset and never computes the full range at all.
    std::vector<const Tile*> validTargets;
    for (const Tile* tile : inRange)
    {
        if (tile->hasEntity() && caster->isValidSpecialTarget(*tile->getEntity()))
            validTargets.push_back(tile);
    }
    highlightTiles(validTargets, caster->getSpecialTargetHighlightColor());
}

// Generic push, N tiles bounded by the CALLER's own maxTiles (see Board.h) -
// no ability-specific number lives here any more. Walks one step at a time,
// stopping at the first tile that's off-board/occupied/impassable; lands on
// the last valid tile reached (nullptr if even the first step is blocked,
// in which case there's no movement at all).
void Board::applyKnockback(BoardEntity* entity, int dq, int dr, int maxTiles) const
{
    if (!entity) return;

    // getMutableTileAt(getQ/getRow) instead of the removed entity->getCurrentTile() - see BoardEntity.h.
    Tile* sourceTile = getMutableTileAt(entity->getQ(), entity->getRow());
    if (!sourceTile) return;

    Tile* current = sourceTile;
    Tile* destination = nullptr;
    for (int step = 0; step < maxTiles; ++step)
    {
        Tile* next = getMutableTileAt(current->getQ() + dq, current->getRow() + dr);
        bool nextValid = next && !next->hasEntity() && next->isPassableFor(entity);
        if (!nextValid) break;

        destination = next;
        current = next;
    }

    if (!destination) return; // first step already blocked/off-board -> no movement at all

    sourceTile->clearEntity();
    destination->setEntity(entity);
    entity->spawnOnBoard(destination->getQ(), destination->getRow(),
        tileToScreen(destination->getQ(), destination->getRow()));
}