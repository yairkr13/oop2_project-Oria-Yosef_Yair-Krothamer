#include "Board.h"
#include "Attacks/AttackAnimation.h"
#include <iostream>
#include <utility>
#include "Constants.h"

Board::Board(const BoardLayout& layout)
    : m_layout(layout), m_pathfinder(m_grid)
{
    createBoard();
}

void Board::createBoard()
{
    // BoardGenerator owns the actual tile-construction knowledge (grid
    // shape, and - later, in generateSpecialTiles - which concrete Tile
    // subtypes exist) - Board just decides *when* this happens (here, at
    // construction) and hands in its own tileAnchor() so the generator
    // never needs to know Board's pixel constants either.
    m_grid = BoardGenerator::buildBaseGrid(m_layout,
        [this](int q, int row) { return tileAnchor(q, row); });
}

void Board::draw(sf::RenderWindow& window) const
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
    for (auto const& [coords, tile] : m_grid)
    {
        tile->draw(window);
    }
    
    // 2. ציור כל הישויות שנמצאות על המשבצות - תמיד מעל כל המשבצות
    for (auto const& [coords, tile] : m_grid)
    {
        tile->drawEntity(window);
    }
}

// בתוך Board.cpp
// בתוך Board.cpp
void Board::initPlayerHearts(Heart* p1Heart, Heart* p2Heart) {
    int middleRow = 3; // השורה האמצעית של הלוח

    // 1. מיקום הלב של שחקן 1 (הכי שמאלי)
    Tile* p1Tile = getLeftmostTileInRow(middleRow);
    if (p1Tile != nullptr && p1Heart != nullptr) {
        p1Heart->spawnOnBoard(p1Tile->getQ(), p1Tile->getRow(), p1Tile->getScreenPosition());
        p1Tile->setEntity(p1Heart);
    }

    // 2. מיקום הלב של שחקן 2 (הכי ימני)
    Tile* p2Tile = getRightmostTileInRow(middleRow);
    if (p2Tile != nullptr && p2Heart != nullptr) {
        p2Heart->spawnOnBoard(p2Tile->getQ(), p2Tile->getRow(), p2Tile->getScreenPosition());
        p2Tile->setEntity(p2Heart);
    }
    generateSpecialTiles(p1Heart, p2Heart);
}

void Board::generateSpecialTiles(Heart* p1Heart, Heart* p2Heart)
{
    BoardGenerator::applySpecialTiles(m_grid, m_layout,
        [this](int q, int row) { return tileAnchor(q, row); },
        p1Heart, p2Heart, rng());
}

bool Board::trySpawnMonster(const sf::Vector2f& pos, Monster* monster)
{
    if (!monster) return false;

    // Same screen-position -> Tile lookup handleClick's replacement
    // (GameplayState::handleBoardClick) and every other click-driven flow
    // already use, instead of a second hand-rolled hypot-distance scan.
    Tile* tile = getTileAtScreenPosition(pos);
    if (!tile || !tile->isHighlighted()) return false;

    if (spawnMonsterOnTile(monster, tile))
    {
        clearHighlights();
        return true;
    }
    return false;
}
void Board::highlightSpawnTiles(PlayerSide side)
{
    clearHighlights();
    for (auto& [coords, tile] : m_grid)
    {
        if (tile->hasEntity()) continue;

        if (side == PlayerSide::Left && coords.first <= 1)
        {
            tile->setHighlighted(true);
        }
        else if (side == PlayerSide::Right && coords.first >= 12)
        {
            tile->setHighlighted(true);
        }
    }
}

// Board stays the public facade for all three of these (see Board.h) - the
// actual BFS now lives entirely in BoardPathfinder (see m_pathfinder and
// BoardPathfinder.h/.cpp). Each is now a one-line forward; no caller of
// Board needed to change.
std::vector<Tile*> Board::getReachableTiles(Monster* monster) const
{
    return m_pathfinder.getReachableTiles(monster);
}

std::vector<Tile*> Board::getExtendedAttackOnlyTiles(Monster* monster) const
{
    return m_pathfinder.getExtendedAttackOnlyTiles(monster);
}

std::vector<Tile*> Board::getPathTo(Monster* monster, Tile* target) const
{
    return m_pathfinder.getPathTo(monster, target);
}

// שכבה 2: wrapper דק - קורא לשאילתה הטהורה למעלה, ורק אחראי על הצביעה (side effect
// ויזואלי). זו הפונקציה שהקליק האנושי קורא לה. AIPlayer, לעומת זאת, יוכל בעתיד לקרוא
// ישירות ל-getReachableTiles ולא לצייר כלום - הוא לא צריך צביעה, רק את הרשימה.
void Board::highlightNeighbors(Monster* monster)
{
    if (!monster) return;

    for (Tile* tile : getReachableTiles(monster))
    {
        //bool isEnemy = tile->hasEntity() && tile->getEntity()->getSide() != monster->getSide();
        if (tile->isOccupiedByEnemy(monster->getSide()))
            tile->setHighlighted(true, sf::Color(255, 90, 90, 180)); // אדום - ניתן לתקוף
        else
            tile->setHighlighted(true); // ירוק (ברירת המחדל) - ניתן לזוז
    }

    // Extended attack-only band (see getExtendedAttackOnlyTiles) - empty for
    // every monster except an empowered Barzilla. Distinct purple, clearly
    // different from both the red attack and green move colors above:
    // "Barzilla can strike here, but cannot move here."
    for (Tile* tile : getExtendedAttackOnlyTiles(monster))
        tile->setHighlighted(true, sf::Color(190, 90, 230, 170));
}

bool Board::selectEntity(BoardEntity* entity, PlayerSide side)
{
    if (!entity || !entity->canBeSelectedBy(side)) return false;

    // Which entity is currently selected is GameplayState's own interaction
    // state (see GameplayState::m_selectedEntity) - Board no longer tracks
    // it, so there's nothing here to clear on any previous selection.
    clearHighlights();
    highlightNeighbors(entity->asMonster());
    return true;
}

void Board::highlightTiles(const std::vector<Tile*>& tiles, const sf::Color& color)
{
    for (Tile* tile : tiles)
    {
        if (tile) tile->setHighlighted(true, color);
    }
}

void Board::clearHighlights()
{
    for (auto& pair : m_grid)
    {
        pair.second->setHighlighted(false);
    }
}

sf::Vector2f Board::tileAnchor(int q, int row) const
{
    float width = std::sqrt(3.f) * Config::TILE_RADIUS;
    return { START_X + (width / 2.f) * q, START_Y + (1.5f * Config::TILE_RADIUS) * row };
}

sf::Vector2f Board::tileToScreen(int q, int row) const
{
    sf::Vector2f anchor = tileAnchor(q, row);
    return { anchor.x + Config::TILE_RADIUS, anchor.y + Config::TILE_RADIUS };
}

void Board::updateTileEffects()
{
    for (auto& [coords, tile] : m_grid)
    {
        tile->applyTileEffect(); // פולימורפיזם בפעולה! משבצת רגילה לא תעשה כלום, לבה תוריד חיים.

        // אם המפלצת מתה מהאפקט (למשל מהלבה), ננקה אותה מהמשבצת
		//no need this anymore because the entity has the tile!!!!!!!!
        if (auto entity = tile->getEntity())
        {
            // Generic per-turn-boundary tick (see BoardEntity::onTurnBoundary) -
            // this is the one existing place a "a player switch just
            // happened" event already reaches every occupied tile, so
            // turn-scoped status effects (Henrietta's Protection, Barzilla's
            // empowered attack) piggyback on it instead of a new timer.
            // Board never learns what any status effect means.
            entity->onTurnBoundary();

            if (entity->isReadyForRemoval()) // כמו ב-receiveAttackFrom - מחכה לאנימציית מוות אם יש
            {
                tile->clearEntity();
            }
        }
    }
}

// החזרת המשבצת הכי שמאלית בשורה מסוימת
Tile* Board::getLeftmostTileInRow(int row) const {
    Tile* leftmost = nullptr;
    int minQ = std::numeric_limits<int>::max();

    for (const auto& [coords, tile] : m_grid) {
        if (coords.second == row) { // אם אנחנו בשורה המבוקשת
            if (coords.first < minQ) {
                minQ = coords.first;
                leftmost = tile.get();
            }
        }
    }
    return leftmost;
}

// החזרת המשבצת הכי ימנית בשורה מסוימת
Tile* Board::getRightmostTileInRow(int row) const {
    Tile* rightmost = nullptr;
    int maxQ = std::numeric_limits<int>::min();

    for (const auto& [coords, tile] : m_grid) {
        if (coords.second == row) { // אם אנחנו בשורה המבוקשת
            if (coords.first > maxQ) {
                maxQ = coords.first;
                rightmost = tile.get();
            }
        }
    }
    return rightmost;
}

// AI_FindBestTargetForMonster עברה ל-AIPlayer::findBestTarget - היא הייתה מכילה
// היוריסטיקה (תקיפה > תנועה שמאלה), לא שאילתה עובדתית, אז לא הייתה שייכת ל-Board.
// AIPlayer עכשיו קורא ל-getReachableTiles() (למעלה בקובץ) בעצמו במקומה.

// שאילתה עובדתית בלבד - "אילו tiles פנויים בשטח הזימון של הצד הזה?" Board לא
// בוחר אף אחת מהן - זו הייתה בדיוק העבודה של AI_SpawnMonster הישנה, שגם אספה
// מועמדים וגם הגרילה אחד מהם. עכשיו רק החלק הראשון (איסוף) נשאר כאן.
std::vector<Tile*> Board::getSpawnableTiles(Monster* monster, PlayerSide side) const
{
    std::vector<Tile*> spawnable;
    if (!monster || monster->isOnBoard()) return spawnable;

    int minQ = (side == PlayerSide::Left) ? 0 : 12;
    int maxQ = (side == PlayerSide::Left) ? 1 : 13;

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
bool Board::spawnMonsterOnTile(Monster* monster, Tile* targetTile)
{
    if (!monster || !targetTile || targetTile->hasEntity() || !targetTile->isPassableFor(monster)) return false;
    targetTile->setEntity(monster);
    monster->spawnOnBoard(targetTile->getQ(), targetTile->getRow(), targetTile->getScreenPosition());
    return true;
}

void Board::performAction(BoardEntity* entity, Tile* targetTile)
{
    if (!entity || !targetTile) return;

    if (targetTile->isOccupiedByEnemy(entity->getSide()))
    {
        performAttack(entity, targetTile);
        return;
    }

    // Movement is Monster-only (Heart has no legal movement) - the one safe
    // downcast performMove needs happens here, once.
    if (Monster* monster = entity->asMonster())
        performMove(monster, targetTile);
}

void Board::performAttack(BoardEntity* entity, Tile* targetTile)
{
    // Give the attacker a chance to supply an animated attack (see
    // BoardEntity::createAttackAnimation). Most entities don't override
    // it, so this is nullptr and the attack resolves immediately below,
    // exactly as before. An entity that does provide one (e.g. Mozzy)
    // gets its damage deferred until the animation reports impact -
    // Board never needs to know which concrete entity/animation this is,
    // nor does it ever compute or inspect a damage value: that stays
    // entirely below Tile::receiveAttackFrom, inside Monster::attack().
    BoardEntity* target = targetTile->getEntity();
    if (std::unique_ptr<AttackAnimation> animation = entity->createAttackAnimation(target))
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

void Board::performMove(Monster* monster, Tile* targetTile)
{
    // Movement is only ever legal onto a tile within this monster's NORMAL
    // range - an extended attack-only range (see Monster::getAttackRange,
    // Barzilla's Empowered Attack) lets it strike farther, never walk
    // farther. Checked explicitly against getReachableTiles here rather
    // than relying on getPathTo coming back empty for such a tile, since
    // the no-path fallback a few lines below would otherwise still
    // teleport the monster there directly.
    std::vector<Tile*> reachable = getReachableTiles(monster);
    bool isMoveLegal = !targetTile->hasEntity() && targetTile->isPassableFor(monster)
        && std::find(reachable.begin(), reachable.end(), targetTile) != reachable.end();

    if (!isMoveLegal) return;

    // לא צריך m_grid.find({q,row}) - המפלצת יודעת ישירות על איזה Tile
    // היא נמצאת, בזכות הקשר הדו-כיווני ב-setEntity/clearEntity.
    Tile* sourceTile = monster->getCurrentTile();
    if (sourceTile == nullptr) return;

    // בונים את המסלול המדורג (משבצת-משבצת) במקום לקפוץ בקו ישר ליעד
    std::vector<Tile*> path = getPathTo(monster, targetTile);
    std::vector<sf::Vector2f> pathScreenPositions;
    pathScreenPositions.reserve(path.size());
    for (Tile* step : path)
        pathScreenPositions.push_back(step->getScreenPosition());

    // רשת ביטחון: אם משום מה לא נמצא מסלול (לא אמור לקרות, כי targetTile
    // כבר אושר כנגיש), נופלים חזרה על תזוזה ישירה כדי שהמפלצת לא "תיתקע"
    if (pathScreenPositions.empty())
        pathScreenPositions.push_back(targetTile->getScreenPosition());

    targetTile->setEntity(monster);
    sourceTile->clearEntity();

    monster->moveAlongPath(targetTile->getQ(), targetTile->getRow(), pathScreenPositions);
}

void Board::update(float dt)
{
    for (auto& [coords, tile] : m_grid)
    {
		//change this to one function!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        if (auto entity = tile->getEntity())
        {
            // Advances this entity's own animation state, whatever it may
            // be - movement (Monster::m_pathQueue) and, now, an in-flight
            // attack animation (Monster::m_attackAnimation) are both driven
            // from the entity's own update() override. Board just relays
            // the per-frame tick; it owns none of that state itself.
            entity->update(dt);

            // A dying entity (see BoardEntity::isDying/isReadyForRemoval)
            // stayed linked to this tile specifically so the tick above
            // could keep advancing its one-shot death animation - now that
            // it's ticked, check whether that animation just finished and,
            // if so, clear it. A no-op for every other entity: one that was
            // never dying is never ready for removal here (it would already
            // have been cleared immediately by Tile::receiveAttackFrom/
            // updateTileEffects instead of ever reaching this loop again).
            if (entity->isReadyForRemoval())
                tile->clearEntity();
        }
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
        if (auto entity = tile->getEntity())
        {
            if (entity->isAnimating()) return true;
            //if (entity->getType() == EntityType::Monster)
            //{
            //    // עכשיו אנחנו בטוחים שזו מפלצת, אז אפשר להמיר בבטחה
            //    Monster* monster = static_cast<Monster*>(entity);
            //    if (monster->isMoving()) return true;
            //}

        }
    }
    return false; // <--- חסר לך את זה! אם אף אחד לא זז, מחזירים שקר
}

std::pair<int, int> Board::screenToTile(const sf::Vector2f& pos) const
{
    // מזיזים את pos למערכת הצירים שעליה tileToScreen בנוי - מחסירים את נקודת
    // המוצא (START_X/START_Y) וגם את TILE_RADIUS ש-tileToScreen מוסיף בסוף כדי
    // למרכז את המשושה (ה-shape עצמו ממוקם לפי הפינה השמאלית-עליונה, לא המרכז).
    float x = pos.x - START_X - Config::TILE_RADIUS;
    float y = pos.y - START_Y - Config::TILE_RADIUS;

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
//
//// Board.cpp - הוספה, לא נוגעת ב-Card בכלל, רק ב-Tile/geometry
bool Board::isSpawnPositionValid(const sf::Vector2f& pos) const
{
    Tile* tile = getTileAtScreenPosition(pos);
    return tile && tile->isHighlighted() && !tile->hasEntity();
}

Tile* Board::getTileAt(int q, int row) const
{
    auto it = m_grid.find({ q, row });
    return (it != m_grid.end()) ? it->second.get() : nullptr;
}

Tile* Board::getTileAtScreenPosition(const sf::Vector2f& pos) const
{
    auto [q, row] = screenToTile(pos);
    return getTileAt(q, row);
}

std::vector<Tile*> Board::getOccupiedTiles() const
{
    std::vector<Tile*> occupied;
    for (auto const& [coords, tile] : m_grid)
    {
        if (tile->hasEntity())
            occupied.push_back(tile.get());
    }
    return occupied;
}