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
    // Smallest board this game supports - a floor, not derived from
    // standardLayout() itself (that would be circular).
    constexpr int MIN_ROWS = 7;
    constexpr int MIN_COLS = 14;

    // Total on-screen footprint, in pixels, for `count` tiles along one axis.
    float boardExtent(int count, float step)
    {
        return step * static_cast<float>(count - 1) + 2.f * Config::TILE_RADIUS;
    }
}

Board::Board(const BoardLayout& layout)
    : m_layout(layout), m_pathfinder(m_grid)
{
    if (m_layout.rows <= 0 || m_layout.cols <= 0)
    {
        throw std::invalid_argument(
            "BoardLayout rows/cols must be positive (got rows=" + std::to_string(m_layout.rows) +
            ", cols=" + std::to_string(m_layout.cols) + ")");
    }

    if (m_layout.rows < MIN_ROWS || m_layout.cols < MIN_COLS)
    {
        throw std::invalid_argument(
            "BoardLayout " + std::to_string(m_layout.rows) + "x" + std::to_string(m_layout.cols) +
            " is smaller than the minimum supported " + std::to_string(MIN_ROWS) + "x" + std::to_string(MIN_COLS));
    }

    // Too few columns makes the two sides' spawn bands overlap or coincide.
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

    // The board only needs to fit above the bottom panel, not the window's full height.
    float playAreaHeight = static_cast<float>(Config::WINDOW_HEIGHT) - Config::BOTTOM_PANEL_HEIGHT;

    if (boardWidth > static_cast<float>(Config::WINDOW_WIDTH) || boardHeight > playAreaHeight)
    {
        throw std::out_of_range(
            "BoardLayout " + std::to_string(m_layout.rows) + "x" + std::to_string(m_layout.cols) +
            " is too large to fit the window (needs " + std::to_string(static_cast<int>(boardWidth)) + "x" +
            std::to_string(static_cast<int>(boardHeight)) + "px, only " +
            std::to_string(Config::WINDOW_WIDTH) + "x" + std::to_string(static_cast<int>(playAreaHeight)) +
            "px available above the bottom panel)");
    }

    // Centered horizontally in the window and vertically in the play area,
    // instead of anchored at a fixed point - see m_startX/m_startY in Board.h.
    m_startX = (static_cast<float>(Config::WINDOW_WIDTH) - boardWidth) / 2.f;
    m_startY = (playAreaHeight - boardHeight) / 2.f;

    createBoard();
}

void Board::createBoard()
{
    // BoardGenerator owns tile-construction knowledge; Board only decides
    // when this happens and supplies its own tileAnchor() callback.
    m_grid = BoardGenerator::buildBaseGrid(m_layout,
        [this](int q, int row) { return tileAnchor(q, row); });
}

void Board::draw(sf::RenderWindow& window, PlayerSide currentSide) const
{
    // Two full passes (all tiles, then all entities) so every occupant draws
    // above every tile regardless of grid iteration order - interleaving the
    // two used to let a tile drawn after a moving entity paint over it.
    for (auto const& [coords, tile] : m_grid)
    {
        tile->draw(window);
    }

    for (auto const& [coords, tile] : m_grid)
    {
        tile->drawEntity(window, currentSide);
    }
}

//void Board::initPlayerHearts(Heart* p1Heart, Heart* p2Heart) {
//    int middleRow = 3; // השורה האמצעית של הלוח
//
//    // 1. מיקום הלב של שחקן 1 (הכי שמאלי)
//    //Tile* p1Tile = getLeftmostTileInRow(middleRow);
//	Tile* p1Tile = getExtremeTileInRow(middleRow, true);
//    if (p1Tile != nullptr && p1Heart != nullptr) {
//        p1Heart->spawnOnBoard(p1Tile->getQ(), p1Tile->getRow(), p1Tile->getScreenPosition());
//        p1Tile->setEntity(p1Heart);
//    }
//
//    // 2. מיקום הלב של שחקן 2 (הכי ימני)
//    //Tile* p2Tile = getRightmostTileInRow(middleRow);
//    Tile* p2Tile = getExtremeTileInRow(middleRow, false);
//    if (p2Tile != nullptr && p2Heart != nullptr) {
//        p2Heart->spawnOnBoard(p2Tile->getQ(), p2Tile->getRow(), p2Tile->getScreenPosition());
//        p2Tile->setEntity(p2Heart);
//    }
//    generateSpecialTiles(p1Heart, p2Heart);
//}
void Board::initPlayerHearts(Heart* p1Heart, Heart* p2Heart, bool useFixedSpecialTiles)
{
    int middleRow = getMiddleRow();

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

//bool Board::trySpawnMonster(const sf::Vector2f& pos, Monster* monster)
//{
//    if (!monster) return false;
//
//    // Same screen-position -> Tile lookup handleClick's replacement
//    // (GameplayState::handleBoardClick) and every other click-driven flow
//    // already use, instead of a second hand-rolled hypot-distance scan.
//    Tile* tile = getTileAtScreenPosition(pos);
//    if (!tile || !tile->isHighlighted()) return false;
//
//    //if (spawnMonsterOnTile(monster, tile))
//    if (spawnEntityOnTile(monster,tile))
//    {
//        clearHighlights();
//        return true;
//    }
//    return false;
//}

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

        if (coords.first >= minQ && coords.first <= maxQ)
        {
            spawnTiles.push_back(tile.get());
        }
    }
    highlightTiles(spawnTiles); // default green, same as before

    /*clearHighlights();
    for (auto& [coords, tile] : m_grid)
    {
        if (tile->hasEntity() || !tile->isPassableFor(monster)) continue;

        if (side == PlayerSide::Left && coords.first <= 1)
        {
            tile->setHighlighted(true);
        }
        else if (side == PlayerSide::Right && coords.first >= 12)
        {
            tile->setHighlighted(true);
        }
    }*/
}

// Board is just the facade here - the actual BFS lives in BoardPathfinder (m_pathfinder).
//std::vector<Tile*> Board::getReachableTiles(Monster* monster) const
//{
//    return m_pathfinder.getReachableTiles(monster);
//}
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

//std::vector<Tile*> Board::getExtendedAttackOnlyTiles(Monster* monster) const
//{
//    return m_pathfinder.getExtendedAttackOnlyTiles(monster);
//}
//
//std::vector<Tile*> Board::getPathTo(Monster* monster, Tile* target) const
//{
//    return m_pathfinder.getPathTo(monster, target);
//}

// Commented out (not deleted): currently always empty now that no monster
// overrides getAttackRange().
//std::vector<const Tile*> Board::getExtendedAttackOnlyTiles(const BoardEntity* entity) const
//{
//    return m_pathfinder.getExtendedAttackOnlyTiles(entity);
//}

std::vector<const Tile*> Board::getPathTo(const BoardEntity* entity, const Tile* target) const
{
    return m_pathfinder.getPathTo(entity, target);
}

// Thin wrapper: calls the pure query above, then paints the result. AIPlayer
// calls getReachableTiles directly instead - it needs the list, not the paint.
//void Board::highlightNeighbors(Monster* monster)
//{
//    if (!monster) return;
//
//    for (Tile* tile : getReachableTiles(monster))
//    {
//        //bool isEnemy = tile->hasEntity() && tile->getEntity()->getSide() != monster->getSide();
//        if (tile->isOccupiedByEnemy(monster->getSide()))
//            tile->setHighlighted(true, sf::Color(255, 90, 90, 180)); // אדום - ניתן לתקוף
//        else
//            tile->setHighlighted(true); // ירוק (ברירת המחדל) - ניתן לזוז
//    }
//
//    // Extended attack-only band (see getExtendedAttackOnlyTiles) - empty for
//    // every monster except an empowered Barzilla. Distinct purple, clearly
//    // different from both the red attack and green move colors above:
//    // "Barzilla can strike here, but cannot move here."
//    for (Tile* tile : getExtendedAttackOnlyTiles(monster))
//        tile->setHighlighted(true, sf::Color(190, 90, 230, 170));
//}
void Board::highlightNeighbors(const BoardEntity* entity) const
{
    if (!entity) return;

    std::vector<const Tile*> enemyTiles, moveTiles;
    for (const Tile* tile : getReachableTiles(entity))
        (tile->isOccupiedByEnemy(entity->getSide()) ? enemyTiles : moveTiles).push_back(tile);

    highlightTiles(moveTiles);                              // green - can move
    highlightTiles(enemyTiles, sf::Color(255, 90, 90, 180)); // red - can attack

    // Extended attack-only band - commented out (not deleted), see
    // getExtendedAttackOnlyTiles: currently always empty.
    //for (const Tile* constTile : getExtendedAttackOnlyTiles(entity))
    //{
    //    Tile* tile = getMutableTileAt(constTile->getQ(), constTile->getRow());
    //    if (tile)
    //        tile->setHighlighted(true, sf::Color(190, 90, 230, 170));
    //}
}

//bool Board::selectEntity(BoardEntity* entity, PlayerSide side)
//{
//    //למה הוא לא שולח מפלצת???? ככה הוא שומר אותה במילא בgamestatee לא?
//    if (!entity || !entity->canBeSelectedBy(side)) return false;
//
//    // Which entity is currently selected is GameplayState's own interaction
//    // state (see GameplayState::m_selectedEntity) - Board no longer tracks
//    // it, so there's nothing here to clear on any previous selection.
//    clearHighlights();
//    highlightNeighbors(entity->asMonster());
//    return true;
//}
bool Board::selectEntity(const BoardEntity* entity, PlayerSide side) const
{
    if (!entity || !entity->canBeSelectedBy(side)) return false;

    clearHighlights();
    highlightNeighbors(entity);
    return true;
}

void Board::highlightTiles(const std::vector<const Tile*>& tiles, const sf::Color& color) const
{
    for (const Tile* constTile : tiles)
    {
        if (!constTile) continue;

        // Look up the real, mutable Tile* by coordinates - queries like
        // getReachableTiles only ever hand back const Tile*.
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
        tile->applyTileEffect(); // plain tile no-ops, lava/etc. deal damage

        // Per-turn-boundary tick for turn-scoped status effects (Protection,
        // Barzilla's empowered attack); tickTurnBoundary also handles its
        // own entity's death cleanup.
        tile->tickTurnBoundary();
    }
}

//Tile* Board::getLeftmostTileInRow(int row) const {
//    Tile* leftmost = nullptr;
//    int minQ = std::numeric_limits<int>::max();
//
//    for (const auto& [coords, tile] : m_grid) {
//        if (coords.second == row) { // אם אנחנו בשורה המבוקשת
//            if (coords.first < minQ) {
//                minQ = coords.first;
//                leftmost = tile.get();
//            }
//        }
//    }
//    return leftmost;
//}
//
//// החזרת המשבצת הכי ימנית בשורה מסוימת
//Tile* Board::getRightmostTileInRow(int row) const {
//    Tile* rightmost = nullptr;
//    int maxQ = std::numeric_limits<int>::min();
//
//    for (const auto& [coords, tile] : m_grid) {
//        if (coords.second == row) { // אם אנחנו בשורה המבוקשת
//            if (coords.first > maxQ) {
//                maxQ = coords.first;
//                rightmost = tile.get();
//            }
//        }
//    }
//    return rightmost;
//}
const Tile* Board::getExtremeTileInRow(int row, bool findLeftmost) const {
    Tile* bestTile = nullptr;
    int bestQ = findLeftmost ? std::numeric_limits<int>::max() : std::numeric_limits<int>::min();

    for (const auto& [coords, tile] : m_grid) {
        if (coords.second == row) {
            if ((findLeftmost && coords.first < bestQ) || (!findLeftmost && coords.first > bestQ)) {
                bestQ = coords.first;
                bestTile = tile.get();
            }
        }
    }
    return bestTile;
}


std::vector<const Tile*> Board::getSpawnableTiles(const Monster* monster, PlayerSide side) const
{
    std::vector<const Tile*> spawnable;
    if (!monster || monster->isOnBoard()) return spawnable;

    auto [minQ, maxQ] = spawnColumnRange(side);

    for (auto& [coords, tile] : m_grid)
    {
        if (coords.first >= minQ && coords.first <= maxQ)
        {
            if (!tile->hasEntity() && tile->isPassableFor(monster))
            {
                spawnable.push_back(tile.get());
            }
        }
    }

    return spawnable;
}
//bool Board::spawnMonsterOnTile(Monster* monster, Tile* targetTile)
//{
//    if (!monster || !targetTile || targetTile->hasEntity() || !targetTile->isPassableFor(monster)) return false;
//    targetTile->setEntity(monster);
//    monster->spawnOnBoard(targetTile->getQ(), targetTile->getRow(), targetTile->getScreenPosition());
//    return true;
//}
bool Board::spawnEntityOnTile(BoardEntity* entity,const Tile* targetTile) const
{
    /*if (!entity || !targetTile || targetTile->hasEntity() || !targetTile->isPassableFor(entity)) return false;*/
    if (!entity || !targetTile) return false;

    // Convert the const Tile* to the board's own internal mutable Tile*.
    Tile* internalTile = getMutableTileAt(targetTile->getQ(), targetTile->getRow());
    if (!internalTile || internalTile->hasEntity() || !internalTile->isPassableFor(entity))
        return false;

    internalTile->setEntity(entity);
    entity->spawnOnBoard(internalTile->getQ(), internalTile->getRow(), internalTile->getScreenPosition());

    SoundPlayer::getInstance().play("summon_sound");
    return true;
}

//void Board::performAction(BoardEntity* entity, Tile* targetTile)
//{
//    if (!entity || !targetTile) return;
//
//    if (targetTile->isOccupiedByEnemy(entity->getSide()))
//    {
//        performAttack(entity, targetTile);
//        return;
//    }
//
//    // Movement is Monster-only (Heart has no legal movement) - the one safe
//    // downcast performMove needs happens here, once.
//    if (Monster* monster = entity->asMonster())//לא אוהבת את זה
//        performMove(monster, targetTile);
//}
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

    if (entity->canMove())
        performMove(entity, targetTile);
}
//void Board::performAction(Monster* monster, Tile* targetTile)
//{
//    if (!monster || !targetTile) return;
//
//    if (targetTile->isOccupiedByEnemy(monster->getSide()))
//    {
//        performAttack(monster, targetTile);
//        return;
//    }
//
//    // Movement is Monster-only (Heart has no legal movement) - the one safe
//    // downcast performMove needs happens here, once.
//    //if (Monster* monster = entity->asMonster())//לא אוהבת את זה
//    performMove(monster, targetTile);
//}

void Board::performAttack(BoardEntity* entity, Tile* targetTile) const
{
    // Attacker may supply an animation (createAttackAnimation); if so, damage
    // is deferred until it reports impact - otherwise the attack resolves immediately.
    if (std::unique_ptr<AttackAnimation> animation = entity->createAttackAnimation(targetTile->getScreenPosition()))
    {
        animation->setOnImpact([targetTile, entity]() {
            targetTile->receiveAttackFrom(entity);
        });

        entity->playAttackAnimation(std::move(animation));
    }
    else
    {
        targetTile->receiveAttackFrom(entity);
    }
}
//void Board::performAttack(Monster* monster, Tile* targetTile)
//{
//    // Give the attacker a chance to supply an animated attack (see
//    // BoardEntity::createAttackAnimation). Most entities don't override
//    // it, so this is nullptr and the attack resolves immediately below,
//    // exactly as before. An entity that does provide one (e.g. Mozzy)
//    // gets its damage deferred until the animation reports impact -
//    // Board never needs to know which concrete entity/animation this is,
//    // nor does it ever compute or inspect a damage value: that stays
//    // entirely below Tile::receiveAttackFrom, inside Monster::attack().
//    BoardEntity* target = targetTile->getEntity();
//    if (std::unique_ptr<AttackAnimation> animation = monster->createAttackAnimation(target))
//    {
//        animation->setOnImpact([targetTile, monster]() {
//            targetTile->receiveAttackFrom(monster);
//            });
//
//        // Board's job ends at deciding the attack happens and wiring
//        // how it eventually resolves - from here the attacker owns and
//        // drives its own animation (update/draw/isAttacking), the same
//        // ownership split it already has for its own movement.
//        monster->playAttackAnimation(std::move(animation));
//    }
//    else
//    {
//        targetTile->receiveAttackFrom(monster);
//    }
//}

//void Board::performMove(Monster* monster, Tile* targetTile)
//{
//    // Movement is only ever legal onto a tile within this monster's NORMAL
//    // range - an extended attack-only range (see Monster::getAttackRange,
//    // Barzilla's Empowered Attack) lets it strike farther, never walk
//    // farther. Checked explicitly against getReachableTiles here rather
//    // than relying on getPathTo coming back empty for such a tile, since
//    // the no-path fallback a few lines below would otherwise still
//    // teleport the monster there directly.
//    std::vector<Tile*> reachable = getReachableTiles(monster);
//    bool isMoveLegal = !targetTile->hasEntity() && targetTile->isPassableFor(monster)
//        && std::find(reachable.begin(), reachable.end(), targetTile) != reachable.end();
//
//    if (!isMoveLegal) return;
//
//    // לא צריך m_grid.find({q,row}) - המפלצת יודעת ישירות על איזה Tile
//    // היא נמצאת, בזכות הקשר הדו-כיווני ב-setEntity/clearEntity.
//    Tile* sourceTile = monster->getCurrentTile();
//    if (sourceTile == nullptr) return;
//
//    // בונים את המסלול המדורג (משבצת-משבצת) במקום לקפוץ בקו ישר ליעד
//    std::vector<Tile*> path = getPathTo(monster, targetTile);
//    std::vector<sf::Vector2f> pathScreenPositions;
//    pathScreenPositions.reserve(path.size());
//    for (Tile* step : path)
//        pathScreenPositions.push_back(step->getScreenPosition());
//
//    // רשת ביטחון: אם משום מה לא נמצא מסלול (לא אמור לקרות, כי targetTile
//    // כבר אושר כנגיש), נופלים חזרה על תזוזה ישירה כדי שהמפלצת לא "תיתקע"
//    if (pathScreenPositions.empty())
//        pathScreenPositions.push_back(targetTile->getScreenPosition());
//
//    targetTile->setEntity(monster);
//    sourceTile->clearEntity();
//
//    monster->moveAlongPath(targetTile->getQ(), targetTile->getRow(), pathScreenPositions);
//}
void Board::performMove(BoardEntity* entity, Tile* targetTile) const
{
    // Legal only onto a tile within normal range - checked against
    // getReachableTiles rather than relying on getPathTo returning empty,
    // since the no-path fallback below would otherwise teleport the entity there.
    std::vector<const Tile*> reachable = getReachableTiles(entity);
    bool isMoveLegal = !targetTile->hasEntity() && targetTile->isPassableFor(entity)
        && std::find(reachable.begin(), reachable.end(), targetTile) != reachable.end();

    if (!isMoveLegal) return;

    Tile* sourceTile = getMutableTileAt(entity->getQ(), entity->getRow());
    if (sourceTile == nullptr) return;

    // Builds the tile-by-tile path instead of jumping straight to the target.
    std::vector<const Tile*> path = getPathTo(entity, targetTile);
    std::vector<sf::Vector2f> pathScreenPositions;
    pathScreenPositions.reserve(path.size());
    for (const Tile* step : path)
        pathScreenPositions.push_back(step->getScreenPosition());

    // Safety net: fall back to a direct move if no path was found (shouldn't happen).
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
        // Relays the per-frame tick to this tile's entity (movement/attack
        // animation); a no-op on an empty tile.
        tile->updateEntity(dt);
    }
}

bool Board::isAnimating() const
{
    for (auto const& [coords, tile] : m_grid)
    {
        if (tile->isEntityAnimating()) return true;
    }
    return false;
}

std::pair<int, int> Board::screenToTile(const sf::Vector2f& pos) const
{
    // Shift into tileToScreen's own coordinate system (subtract m_startX/Y and TILE_RADIUS).
    float x = pos.x - m_startX - Config::TILE_RADIUS;
    float y = pos.y - m_startY - Config::TILE_RADIUS;

    // Axial -> cube coordinates, then round and correct the largest-error axis.
    float q_axial_frac = (std::sqrt(3.f) / 3.f * x - 1.f / 3.f * y) / Config::TILE_RADIUS;
    float r_axial_frac = (2.f / 3.f * y) / Config::TILE_RADIUS;

    float x_cube = q_axial_frac;
    float z_cube = r_axial_frac;
    float y_cube = -x_cube - z_cube;

    int rx = static_cast<int>(std::round(x_cube));
    int ry = static_cast<int>(std::round(y_cube));
    int rz = static_cast<int>(std::round(z_cube));

    float x_diff = std::abs(rx - x_cube);
    float y_diff = std::abs(ry - y_cube);
    float z_diff = std::abs(rz - z_cube);

    if (x_diff > y_diff && x_diff > z_diff)
        rx = -ry - rz;
    else if (y_diff > z_diff)
        ry = -rx - rz;
    else
        rz = -rx - ry;

    int qAxial = rx;
    int row = rz;

    // axial -> doubled coordinates (the q convention this project actually uses).
    int qDouble = 2 * qAxial + row;

    return { qDouble, row };
}

const Tile* Board::pickRandomTile(const std::vector<const Tile*>& tiles) const
{
    if (tiles.empty()) return nullptr;

    std::uniform_int_distribution<size_t> dist(0, tiles.size() - 1);
    return tiles[dist(rng())];
}

const Tile* Board::getTileAt(int q, int row) const
{
    return getMutableTileAt(q, row);
    //auto it = m_grid.find({ q, row });
    //return (it != m_grid.end()) ? it->second.get() : nullptr;
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

// Commented out (not deleted) - now bounds candidates by range (getReachableTiles) instead.
//std::vector< Tile*> Board::getOccupiedTiles() const
//{
//    std::vector<Tile*> occupied;
//    for (auto const& [coords, tile] : m_grid)
//    {
//        if (tile->hasEntity())
//            occupied.push_back(tile.get());
//    }
//    return occupied;
//}

void Board::highlightValidSpecialTargets(const Monster* caster) const
{
    if (!caster) return;

    // Bounded by range (same reachability query move/attack highlighting
    // uses). includeAllies=true: ally-targeted Specials need allies visible too.
    std::vector<const Tile*> inRange = getReachableTiles(caster, /*includeAllies=*/true);

    // Whole range shown first, at half alpha, in the caster's own Special color.
    sf::Color rangeColor = caster->getSpecialTargetHighlightColor();
    rangeColor.a /= 2;
    highlightTiles(inRange, rangeColor);

    // Then the actual valid targets, at full color, on top.
    std::vector<const Tile*> validTargets;
    for (const Tile* tile : inRange)
    {
        if (tile->hasEntity() && caster->isValidSpecialTarget(*tile->getEntity()))
            validTargets.push_back(tile);
    }
    highlightTiles(validTargets, caster->getSpecialTargetHighlightColor());
}

void Board::applyKnockback(BoardEntity* entity, int dq, int dr, int maxTiles) const
{
    if (!entity) return;

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
