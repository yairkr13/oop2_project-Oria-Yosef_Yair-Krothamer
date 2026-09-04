#include "AIPlayer.h"
#include "Tiles/Tile.h"
#include <iostream>

AIPlayer::AIPlayer(PlayerSide side)
    : Player(side)
{
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
        if (tile->hasEntity() && tile->isOccupiedByEnemy(getSide()))
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
    // Phase 1: Spawn all affordable monsters immediately (spawning is instant, no animation)
    for (auto& cardPtr : m_hand)
    {
        Card* card = cardPtr.get();
        
        if (!card || card->isPlayed() || card->getCost() > m_keys)
            continue;

        Monster* monster = playCard(card);
        std::vector<Tile*> candidates = board.getSpawnableTiles(monster,getSide());
        if (candidates.empty())
            break;

        std::uniform_int_distribution<size_t> dist(0, candidates.size() - 1);
        Tile* chosenTile = candidates[dist(Board::rng())];

        // playCard מוריד מפתחות, מייצר Monster ומכניס ל-m_monsters
        
        if (monster)
        {
            board.spawnMonsterOnTile(monster, chosenTile);
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