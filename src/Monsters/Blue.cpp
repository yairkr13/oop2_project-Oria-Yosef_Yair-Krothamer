#include "Monsters/Blue.h"
#include "Attacks/BurstProjectileAnimation.h"
#include "Attacks/SpinningProjectileAnimation.h"
#include "AssetsManager.h"
#include "Constants.h"
#include "Board.h"
#include "Tiles/Tile.h"
#include <limits>
#include <utility>

namespace
{
    // Normal-attack burst (WindBlast): several rapid shots, staggered, only
    // the last one deals damage - see BurstProjectileAnimation.
    constexpr int WIND_BLAST_COUNT = 5;
    constexpr float WIND_BLAST_LAUNCH_INTERVAL = 0.08f; // seconds between successive launches
    constexpr float WIND_BLAST_TRAVEL_DURATION = 0.35f; // seconds each individual shot takes to arrive
    constexpr float WIND_BLAST_SIZE = Config::MONSTER_BOARD_SIZE * 0.35f; // smaller per-shot than a full monster, several fly together

    // Knockback wind effect: travels through the target along the same
    // direction it's about to be knocked back in - approaching from roughly
    // one tile on the opposite side and finishing about half a tile past
    // the target's original position, so it reads as the force pushing it
    // away rather than a static hit. Both distances are fractions of the
    // actual on-screen hex-step vector for that direction (computed from
    // Board::tileToScreen at the call site), not guessed pixel values.
    constexpr float WIND_EFFECT_APPROACH_TILES = 1.0f;
    constexpr float WIND_EFFECT_OVERSHOOT_TILES = 0.5f;
    constexpr float WIND_EFFECT_DURATION = 0.4f;
    constexpr float WIND_EFFECT_SIZE = Config::MONSTER_BOARD_SIZE * 0.9f;

    // Walking/movement sprite sheet: Blue is a flying monster, so its
    // BlueFly.png sheet is used for its board-travel animation instead of a
    // ground walk cycle - same mechanism either way (Monster::setWalkAnimation
    // doesn't care what the motion depicts, only that it loops while moving).
    // 6 columns x 4 rows (24 frames), verified against the actual file, same
    // layout as Muffintop's working sheet.
    constexpr int WALK_SHEET_COLUMNS = 6;
    constexpr int WALK_SHEET_ROWS = 4;
    constexpr float WALK_FRAME_DURATION = 0.06f;

    // Knockback direction math: Blue's Special can target ANY enemy Monster
    // on the Board (Monster::isValidSpecialTarget's default is "any enemy",
    // not "adjacent enemy" - see Monster.h), so (target.q - Blue.q, target.row
    // - Blue.row) is generally NOT one of the board's six hex-neighbor
    // offsets, only a multiple/approximation of one. This is a self-contained
    // conversion into cube coordinates (the standard way to reason about
    // direction on a hex grid) purely to answer "which single hex-step
    // continues outward along the Blue->target line" - it does not touch or
    // duplicate any Board rule (occupancy/passability/ally-enemy all still
    // come from Board::getTileAt + Tile, unchanged below).
    struct CubeCoord { int x, y, z; };

    // This board's (q, row) is a "doubled" coordinate system: q always
    // moves in steps of 2 for a same-row neighbor, or +-1 alongside a +-1
    // row change for a diagonal neighbor (q - row is always even). That is
    // exactly the standard doubled-width <-> cube coordinate mapping.
    CubeCoord toCube(int q, int row)
    {
        int x = (q - row) / 2;
        int z = row;
        return { x, -x - z, z };
    }

    // The six unit directions of a cube-coordinate hex grid - a fixed fact
    // of the coordinate system itself (every permutation of 1, -1, 0).
    constexpr int HEX_DIRECTIONS[6][3] = {
        { 1, -1,  0}, {-1,  1,  0},
        { 1,  0, -1}, {-1,  0,  1},
        { 0,  1, -1}, { 0, -1,  1}
    };

    // The single hex-step (dq, dr) that best continues outward along the
    // line from (attackerQ,attackerRow) to (targetQ,targetRow): exact when
    // the two tiles are aligned along one of the six axes (as in the
    // knockback example in the design), and the closest matching direction
    // otherwise. Chosen by maximizing the dot product against each of the
    // six unit directions - the standard way to pick the nearest hex
    // direction for an arbitrary vector.
    std::pair<int, int> knockbackStep(int attackerQ, int attackerRow, int targetQ, int targetRow)
    {
        CubeCoord from = toCube(attackerQ, attackerRow);
        CubeCoord to = toCube(targetQ, targetRow);
        int dx = to.x - from.x, dy = to.y - from.y, dz = to.z - from.z;

        int bestIndex = 0;
        int bestDot = std::numeric_limits<int>::min();
        for (int i = 0; i < 6; ++i)
        {
            int dot = dx * HEX_DIRECTIONS[i][0] + dy * HEX_DIRECTIONS[i][1] + dz * HEX_DIRECTIONS[i][2];
            if (dot > bestDot) { bestDot = dot; bestIndex = i; }
        }

        // Inverse of toCube: q = 2x + z, row = z.
        int ddx = HEX_DIRECTIONS[bestIndex][0];
        int ddz = HEX_DIRECTIONS[bestIndex][2];
        return { 2 * ddx + ddz, ddz };
    }
}

Blue::Blue(PlayerSide side)
    : Monster(side, "Blue", BASE_HEALTH, BASE_ATTACK, BASE_RANGE, BASE_COOLDOWN, -1, -1, sf::Color::Magenta, "blue")
{
    setWalkAnimation("blue_fly", WALK_SHEET_COLUMNS, WALK_SHEET_ROWS, WALK_FRAME_DURATION);
}

std::unique_ptr<AttackAnimation> Blue::createAttackAnimation(BoardEntity* target) const
{
    if (!target) return nullptr;

    const sf::Texture& windBlastTexture = AssetsManager::getInstance().getTexture("wind_blast");
    return std::make_unique<BurstProjectileAnimation>(
        windBlastTexture, m_screenPos, target->getScreenPosition(),
        WIND_BLAST_COUNT, WIND_BLAST_LAUNCH_INTERVAL, WIND_BLAST_TRAVEL_DURATION, WIND_BLAST_SIZE);
}

// Knockback: pushes the target up to 2 tiles directly away from Blue.
// "Backward" is defined as continuing along the attacker->target direction.
// Blue's Special can target ANY enemy Monster on the Board (see
// isValidSpecialTarget's default in Monster.h - no adjacency requirement),
// so that direction is generally NOT the raw (target - Blue) coordinate
// delta; knockbackStep() above resolves it to a proper single hex-step
// first. Passability (including the existing Hole/flying rule) and
// occupancy are both reused as-is from Tile - Blue adds no new board rule,
// only the meaning of "knockback" itself.
void Blue::onSpecialAbility(Board& board, BoardEntity* target)
{
    Monster* targetMonster = target ? target->asMonster() : nullptr;
    if (!targetMonster) return;

    Tile* currentTile = targetMonster->getCurrentTile();
    if (!currentTile) return;

    auto [dq, dr] = knockbackStep(m_q, m_row, targetMonster->getQ(), targetMonster->getRow());

    // Wind effect: built from the target's ORIGINAL position and the
    // board's own tile-to-tile screen distance for (dq, dr) - the exact
    // same direction the knockback below moves in - before anything is
    // repositioned. Plays regardless of whether the push itself ends up
    // blocked, same as the ability's action/cooldown already commit
    // regardless (see Monster::useSpecialAbility): the wind still visually
    // hits the target either way.
    sf::Vector2f originalScreenPos = targetMonster->getScreenPosition();
    sf::Vector2f stepVector = board.tileToScreen(targetMonster->getQ() + dq, targetMonster->getRow() + dr)
        - board.tileToScreen(targetMonster->getQ(), targetMonster->getRow());

    sf::Vector2f windOrigin = originalScreenPos - stepVector * WIND_EFFECT_APPROACH_TILES;
    sf::Vector2f windTarget = originalScreenPos + stepVector * WIND_EFFECT_OVERSHOOT_TILES;

    const sf::Texture& windEffectTexture = AssetsManager::getInstance().getTexture("wind_effect");
    auto windEffect = std::make_unique<SpinningProjectileAnimation>(
        windEffectTexture, windOrigin, windTarget, WIND_EFFECT_DURATION, WIND_EFFECT_SIZE, 0.f);

    // The actual knockback move is deferred to the moment the wind visually
    // reaches windTarget - SpinningProjectileAnimation already calls
    // fireImpact() exactly then - so the enemy stays put while the wind is
    // still traveling and only flies backward once it "hits". Re-fetches
    // the target's current tile at fire-time (rather than capturing
    // currentTile from above) since this now runs later, not immediately.
    windEffect->setOnImpact([&board, targetMonster, dq, dr]() {
        Tile* sourceTile = targetMonster->getCurrentTile();
        if (!sourceTile) return;

        Tile* step1 = board.getTileAt(targetMonster->getQ() + dq, targetMonster->getRow() + dr);
        bool step1Valid = step1 && !step1->hasEntity() && step1->isPassableFor(targetMonster);
        if (!step1Valid) return; // first tile blocked/off-board -> no movement at all

        Tile* step2 = board.getTileAt(step1->getQ() + dq, step1->getRow() + dr);
        bool step2Valid = step2 && !step2->hasEntity() && step2->isPassableFor(targetMonster);

        Tile* destination = step2Valid ? step2 : step1; // push 2 if both clear, otherwise exactly 1

        sourceTile->clearEntity();
        destination->setEntity(targetMonster);
        targetMonster->spawnOnBoard(destination->getQ(), destination->getRow(),
            board.tileToScreen(destination->getQ(), destination->getRow()));
    });

    targetMonster->playSpecialAbilityAnimation(std::move(windEffect));
}