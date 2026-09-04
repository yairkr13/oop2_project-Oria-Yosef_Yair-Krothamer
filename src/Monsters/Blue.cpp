#include "Monsters/Blue.h"
#include "Attacks/BurstProjectileAnimation.h"
#include "Attacks/SpinningProjectileAnimation.h"
#include "AssetsManager.h"
#include "Constants.h"
#include "Board.h"
#include "Tiles/Tile.h"
#include "HexGrid.h"

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

    // Idle sprite sheet: 6x4 (24 frames), verified against the actual file -
    // same grid as every other sheet in the project, inspected directly
    // rather than assumed. Same calm pace as Muffintop's Idle.
    constexpr int IDLE_SHEET_COLUMNS = 6;
    constexpr int IDLE_SHEET_ROWS = 4;
    constexpr float IDLE_FRAME_DURATION = 0.08f;

    // Attack sprite sheet: same 6x4 grid. Unlike Muffintop's single-shot
    // MUFFIN_SHOT_DURATION, Blue's own attack (see createAttackAnimation
    // below) is a 5-shot burst: the last of WIND_BLAST_COUNT shots launches
    // at (WIND_BLAST_COUNT-1)*WIND_BLAST_LAUNCH_INTERVAL = 0.32s, then takes
    // WIND_BLAST_TRAVEL_DURATION = 0.35s more to land - isAttacking() (what
    // drives this sheet) stays true for that whole ~0.67s window. 24 frames
    // at 0.028s each finishes in ~0.672s, matching it closely instead of
    // Muffintop's frame duration, which would finish far too early here.
    constexpr int ATTACK_SHEET_COLUMNS = 6;
    constexpr int ATTACK_SHEET_ROWS = 4;
    constexpr float ATTACK_FRAME_DURATION = 0.028f;

    // Die sprite sheet: same 6x4 grid, non-looping (setDieSpriteAnimation
    // always configures looping=false - see Monster). Same pace as
    // Muffintop's Die: ~1.2s, a clearly readable one-shot collapse.
    constexpr int DIE_SHEET_COLUMNS = 6;
    constexpr int DIE_SHEET_ROWS = 4;
    constexpr float DIE_FRAME_DURATION = 0.05f;
}

Blue::Blue(PlayerSide side)
    : Monster(side, "Blue", BASE_HEALTH, BASE_ATTACK, BASE_RANGE, BASE_COOLDOWN, -1, -1, sf::Color::Magenta, "blue")
{
    setWalkAnimation("blue_fly", WALK_SHEET_COLUMNS, WALK_SHEET_ROWS, WALK_FRAME_DURATION);
    setIdleSpriteAnimation("blue_idle", IDLE_SHEET_COLUMNS, IDLE_SHEET_ROWS, IDLE_FRAME_DURATION);
    setAttackSpriteAnimation("blue_attack", ATTACK_SHEET_COLUMNS, ATTACK_SHEET_ROWS, ATTACK_FRAME_DURATION);
    setDieSpriteAnimation("blue_die", DIE_SHEET_COLUMNS, DIE_SHEET_ROWS, DIE_FRAME_DURATION);
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
// delta; HexGrid::stepToward resolves it to a proper single hex-step first -
// the same shared hex-direction math Board/BoardPathfinder's own
// reachability BFS is built on (see HexGrid.h), so this knowledge exists in
// exactly one place instead of Blue re-deriving its own copy. Passability
// (including the existing Hole/flying rule) and occupancy are both reused
// as-is from Tile - Blue adds no new board rule, only the meaning of
// "knockback" itself.
void Blue::onSpecialAbility(Board& board, BoardEntity* target)
{
    Monster* targetMonster = target ? target->asMonster() : nullptr;
    if (!targetMonster) return;

    Tile* currentTile = targetMonster->getCurrentTile();
    if (!currentTile) return;

    auto [dq, dr] = HexGrid::stepToward(m_q, m_row, targetMonster->getQ(), targetMonster->getRow());

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