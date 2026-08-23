#include "Monsters/Blue.h"
#include "Attacks/BurstProjectileAnimation.h"
#include "AssetsManager.h"
#include "Constants.h"
#include "Board.h"
#include "Tile.h"
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
    //TextureManager::getInstance().loadTexture("blue_r", "resources/Monster/Blue/Blue_R.png");
    //TextureManager::getInstance().loadTexture("blue_l", "resources/Monster/Blue/Blue_L.png");
    //TextureManager::getInstance().loadTexture("blue_card_r", "resources/Monster/Blue/Blue_card_R.png");
    //TextureManager::getInstance().loadTexture("blue_card_l", "resources/Monster/Blue/Blue_card_L.png");
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

    Tile* step1 = board.getTileAt(targetMonster->getQ() + dq, targetMonster->getRow() + dr);
    bool step1Valid = step1 && !step1->hasEntity() && step1->isPassableFor(targetMonster);
    if (!step1Valid) return; // first tile blocked/off-board -> no movement at all

    Tile* step2 = board.getTileAt(step1->getQ() + dq, step1->getRow() + dr);
    bool step2Valid = step2 && !step2->hasEntity() && step2->isPassableFor(targetMonster);

    Tile* destination = step2Valid ? step2 : step1; // push 2 if both clear, otherwise exactly 1

    currentTile->clearEntity();
    destination->setEntity(targetMonster);
    targetMonster->spawnOnBoard(destination->getQ(), destination->getRow(),
        board.tileToScreen(destination->getQ(), destination->getRow()));
}