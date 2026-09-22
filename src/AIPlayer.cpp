#include "AIPlayer.h"
#include "Tiles/Tile.h"
#include "HexGrid.h"
#include <iostream>
#include <limits>

AIPlayer::AIPlayer(PlayerSide side)
    : Player(side)
{
}

const Tile* AIPlayer::findBestTarget(const Board& board, Monster* monster) const
{
    if (!monster || !monster->isAlive()) return nullptr;

	//ask the board what tiles are reachable for this monster, and then the AI will decide what to do with them
    std::vector<const Tile*> reachable = board.getReachableTiles(monster);

    const Tile* bestAttackTarget = nullptr;
    float bestAttackScore = -std::numeric_limits<float>::infinity();
    const Tile* bestMoveTarget = nullptr;
    int bestMoveDistance = std::numeric_limits<int>::max();

    // Where this monster advances toward when it has nothing to attack -
    // the enemy's own back row. Board::initPlayerHearts always places each
    // side's Heart at the extreme tile of the board's middle row - this
    // asks for that same tile purely by geometry (middle row + which side's
    // extreme), never by asking Board "where is the enemy's Heart".
    bool enemyIsLeft = (getSide() == PlayerSide::Right);
    const Tile* goalTile = board.getExtremeTileInRow(board.getMiddleRow(), enemyIsLeft);

    for (const Tile* tile : reachable)
    {
		//first priority: if there's an enemy in range, attack the best-scoring one
        if (tile->hasEntity() && tile->isOccupiedByEnemy(getSide()))
        {
            float score = tile->scoreAsAttackTarget();
            if (score > bestAttackScore)
            {
                bestAttackScore = score;
                bestAttackTarget = tile;
            }
            continue; // still need to check the remaining enemies' scores
        }

		//second priority: if there's an empty tile, move to whichever gets closest to goalTile
        if (!tile->hasEntity() && tile->isPassableFor(monster))
        {
            int dist = goalTile
                ? HexGrid::distance(tile->getQ(), tile->getRow(), goalTile->getQ(), goalTile->getRow())
                : tile->getQ(); // no goal tile (shouldn't happen) - falls back to the old Q-only heuristic

            if (!bestMoveTarget || dist < bestMoveDistance)
            {
                bestMoveDistance = dist;
                bestMoveTarget = tile;
            }
        }
    }

	// return the best attack target if it exists, otherwise return the best move target
    return bestAttackTarget ? bestAttackTarget : bestMoveTarget;
}

const Tile* AIPlayer::findBestSpecialTarget(const Board& board, Monster* monster) const
{
    if (!monster || !monster->canUseSpecialAbilityNow()) return nullptr;

    // Only ever occupied tiles - a Special never targets an empty tile, so
    // there's no reason to look at getReachableTiles' full range here (that
    // one matters for findBestTarget above, where an empty tile IS a valid
    // move destination). includeAllies=true - Muffintop/Henrietta/Barzilla's
    // Specials target allies, who'd otherwise never appear at all (see
    // Board::getReachableTiles).
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
    // Phase 1: Spawn all affordable monsters immediately (spawning is instant, no animation)
    for (auto& cardPtr : m_hand)
    {
        Card* card = cardPtr.get();
        
        if (!card || card->isPlayed() || card->getCost() > m_keys)
            continue;

        Monster* monster = playCard(card);
        std::vector<const Tile*> candidates = board.getSpawnableTiles(monster,getSide());
        if (candidates.empty())
            break;

        std::uniform_int_distribution<size_t> dist(0, candidates.size() - 1);
        const Tile* chosenTile = candidates[dist(Board::rng())];

        // playCard מוריד מפתחות, מייצר Monster ומכניס ל-m_monsters
        
        if (monster)
        {
            //board.spawnMonsterOnTile(monster, chosenTile);
            board.spawnEntityOnTile(monster, chosenTile);
        }
    }

    // שלב 2: הכנה לשלב הפעולה - נתחיל מהמפלצת הראשונה
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

        // Try the Special before a normal move/attack - mirrors the priority
        // GameplayState leaves to the human player's own choice, but the AI
        // has to decide it itself: use the Special this action if it can and
        // there's something worth using it on.
        if (monster->canUseSpecialAbilityNow())
        {
            if (!monster->specialAbilityNeedsTarget())
            {
                // Self/no-target Special (none currently exist, but the hook
                // stays generic - see Monster::specialAbilityNeedsTarget) -
                // no target search needed at all, just commit it.
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
            // No valid target in range right now - fall through to a normal
            // move/attack instead of wasting this action doing nothing.
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
        std::cout << "[AI] Performing action with monster at (" << monster->getQ() << "," << monster->getRow() << ")" << std::endl;
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

