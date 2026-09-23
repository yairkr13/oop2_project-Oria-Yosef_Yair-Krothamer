#include "AIPlayer.h"
#include "Tiles/Tile.h"
#include "HexGrid.h"
#include <limits>

namespace
{
    // Large enough to always outrank a non-lethal score (never exceeds -1),
    // small enough to never approach Heart's own infinity score.
    constexpr float LETHAL_HIT_BONUS = 100000.f;
}

AIPlayer::AIPlayer(PlayerSide side)
    : Player(side)
{
}

const Tile* AIPlayer::findBestTarget(const Board& board, const Monster* monster) const
{
    // Nothing to do for a missing or already-dead monster.
    if (!monster || !monster->isAlive()) return nullptr;

    std::vector<const Tile*> reachable = board.getReachableTiles(monster);

    const Tile* bestAttackTarget = nullptr;
    float bestAttackScore = -std::numeric_limits<float>::infinity();
    const Tile* bestMoveTarget = nullptr;
    int bestMoveDistance = std::numeric_limits<int>::max();

    // Where this monster advances when it has nothing to attack - the
    // enemy's back row, found by pure geometry (middle row + extreme tile),
    // never by asking Board where the enemy's Heart is.
    bool enemyIsLeft = (getSide() == PlayerSide::Right);
    const Tile* goalTile = board.getExtremeTileInRow(board.getMiddleRow(), enemyIsLeft);

    for (const Tile* tile : reachable)
    {
        // First priority: attack the best-scoring enemy in range.
        if (tile->hasEntity() && tile->isOccupiedByEnemy(getSide()))
        {
            // A lethal hit gets a large flat bonus on top of its own score,
            // so it always outranks a non-lethal one while still preferring
            // the lowest-health kill among lethal candidates.
            float score = tile->scoreAsAttackTarget();
            bool wouldKill = (score + monster->getAttackDamage() >= 0.f);
            float finalScore = wouldKill ? (score + LETHAL_HIT_BONUS) : score;

            if (finalScore > bestAttackScore)
            {
                bestAttackScore = finalScore;
                bestAttackTarget = tile;
            }
            continue; // still need to check the remaining enemies' scores
        }

        // Otherwise: move to whichever empty tile gets closest to goalTile.
        if (!tile->hasEntity() && tile->isPassableFor(monster))
        {
            int dist = goalTile
                ? HexGrid::distance(tile->getQ(), tile->getRow(), goalTile->getQ(), goalTile->getRow())
                : tile->getQ(); // no goal tile (shouldn't happen) - falls back to the old Q-only heuristic

            // First candidate found, or a strictly closer one than what's
            // been picked so far.
            if (!bestMoveTarget || dist < bestMoveDistance)
            {
                bestMoveDistance = dist;
                bestMoveTarget = tile;
            }
        }
    }

    return bestAttackTarget ? bestAttackTarget : bestMoveTarget;
}

const Tile* AIPlayer::findBestSpecialTarget(const Board& board, const Monster* monster) const
{
    // Nothing to search for if there's no monster, or its Special isn't
    // usable right now (dead, on cooldown, etc.).
    if (!monster || !monster->canUseSpecialAbilityNow()) return nullptr;

    // Only occupied tiles - a Special never targets an empty one.
    // includeAllies=true: ally-targeted Specials need allies visible too.
    const Tile* best = nullptr;
    float bestScore = -std::numeric_limits<float>::infinity();
    for (const Tile* tile : board.getReachableOccupiedTiles(monster, /*includeAllies=*/true))
    {
        const BoardEntity* candidate = tile->getEntity();
        if (!monster->isValidSpecialTarget(*candidate))
            continue;

        float score = monster->scoreAsSpecialTarget(*candidate);
        if (score > bestScore)
        {
            bestScore = score;
            best = tile;
        }
    }
    return best;
}

void AIPlayer::onTurnStart(Board& board)
{
    // Phase 1: spawn every affordable monster immediately (spawning has no animation).
    for (auto& cardPtr : m_hand)
    {
        Card* card = cardPtr.get();

        // Skip a missing/already-played card, or one this AI can't currently afford.
        if (!card || card->isPlayed() || card->getCost() > m_keys)
            continue;

        Monster* monster = playCard(card);
        std::vector<const Tile*> candidates = board.getSpawnableTiles(monster,getSide());
        if (candidates.empty())
            break;

        const Tile* chosenTile = board.pickRandomTile(candidates);

        if (monster)
        {
            board.spawnEntityOnTile(monster, chosenTile);
        }
    }

    // Phase 2: start the acting phase, from the first monster.
    m_phase = AITurnPhase::Acting;
    m_currentMonsterIdx = 0;
    m_safetyCounter = 0;
}

void AIPlayer::updateTurn(Board& board)
{
    if (m_phase == AITurnPhase::Done)
        return ;

    // Wait for any active animation to finish before executing next action
    if (board.isAnimating())
        return ;

    // Find the current monster to act with
    while (m_currentMonsterIdx < static_cast<int>(m_monsters.size()))
    {
        Monster* monster = m_monsters[m_currentMonsterIdx].get();

        // Skip dead/off-board monsters
        if (!monster || !monster->isAlive() || !monster->isOnBoard())
        {
            m_currentMonsterIdx++;
            m_safetyCounter = 0;
            continue;
        }
        // Check if this monster still has actions
        if (monster->getActionsLeft() <= 0 || m_safetyCounter >= 5)
        {
            m_currentMonsterIdx++;
            m_safetyCounter = 0;
            continue;
        }

        // Try the Special before a normal move/attack - the AI has to
        // decide this itself, mirroring the choice a human player would make.
        if (monster->canUseSpecialAbilityNow())
        {
            if (!monster->specialAbilityNeedsTarget())
            {
                // Self/no-target Special - no target search needed, just commit it.
                m_safetyCounter++;
                monster->useSpecialAbility(board);
                return;
            }

            const Tile* specialTarget = findBestSpecialTarget(board, monster);
            if (specialTarget)
            {
                m_safetyCounter++;
                monster->useSpecialAbility(board, specialTarget->getMutableEntity());
                return;
            }
            // No valid target in range - fall through to a normal move/attack instead.
        }

        // Find best target for this monster
        const Tile* targetTile = findBestTarget(board, monster);

        if (!targetTile)
        {
            // No valid target, move to next monster
            m_currentMonsterIdx++;
            m_safetyCounter = 0;
            continue;
        }

        // Execute this single action (will trigger walkTo animation for moves)
        m_safetyCounter++;
        board.performAction(monster, targetTile);

        // Return - we executed one action, now wait for animation to finish
        return;
    }

    // All monsters processed - AI turn is done
    m_phase = AITurnPhase::Done;
}

bool AIPlayer::isBusy() const
{
    return m_phase != AITurnPhase::Done;
}
