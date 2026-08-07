#include "AIPlayer.h"
#include "Tile.h"
#include <iostream>

AIPlayer::AIPlayer(PlayerSide side)
    : Player(side)
{
}

void AIPlayer::beginTurn(Board& board)
{
    std::cout << "[AI] Starting turn..." << std::endl;

    // Phase 1: Spawn all affordable monsters immediately (spawning is instant, no animation)
    for (auto& monsterPtr : m_monsters)
    {
        Monster* monster = monsterPtr.get();

        if (!monster->isOnBoard() && monster->isAlive() && monster->getCost() <= m_keys)
        {
            if (board.AI_SpawnMonster(monster, getSide()))
            {
                reduceKeys(monster->getCost());
                std::cout << "[AI] Spawned monster, keys left: " << m_keys << std::endl;
            }
            else
            {
                break;
            }
        }
    }

    // Prepare for action phase: start from first monster
    m_phase = AITurnPhase::Acting;
    m_currentMonsterIdx = 0;
    m_safetyCounter = 0;
}

bool AIPlayer::advanceTurn(Board& board)
{
    if (m_phase == AITurnPhase::Done)
        return true;

    // Wait for any active animation to finish before executing next action
    if (board.isAnimating())
        return false;

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

        // Find best target for this monster
        Tile* targetTile = findBestTarget(board, monster);

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

        // Return false � we executed one action, now wait for animation to finish
        return false;
    }

    // All monsters processed � AI turn is done
    m_phase = AITurnPhase::Done;
    std::cout << "[AI] Finished turn." << std::endl;
    return true;
}

Tile* AIPlayer::findBestTarget(Board& board, Monster* monster) const
{
    if (!monster || !monster->isAlive()) return nullptr;

    // שאילתה עובדתית בלבד - "מה נגיש?" - Board לא בוחר כלום כאן, רק מחזיר רשימה.
    std::vector<Tile*> reachable = board.getReachableTiles(monster);

    Tile* bestAttackTarget = nullptr;
    Tile* bestMoveTarget = nullptr;

    for (Tile* tile : reachable)
    {
        // עדיפות א': אויב בטווח - תוקפים אותו
        if (tile->hasEntity() && tile->getEntity()->getSide() != monster->getSide())
        {
            bestAttackTarget = tile;
            break; // מצאנו מטרה, אין צורך להמשיך לחפש
        }

        // עדיפות ב': משבצת פנויה שמתקדמת שמאלה (Q קטן יותר)
        if (!tile->hasEntity() && tile->isPassableFor(monster))
        {
            if (!bestMoveTarget || tile->getQ() < bestMoveTarget->getQ())
            {
                bestMoveTarget = tile;
            }
        }
    }

    // נחזיר קודם כל תקיפה, ואם אין - תנועה
    return bestAttackTarget ? bestAttackTarget : bestMoveTarget;
}

void AIPlayer::onTurnStart(Board& board)
{
    beginTurn(board);
}

void AIPlayer::updateTurn(Board& board)
{
    advanceTurn(board);
}

bool AIPlayer::isBusy() const
{
    return m_phase != AITurnPhase::Done;
}