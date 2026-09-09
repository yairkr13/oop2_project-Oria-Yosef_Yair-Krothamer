#include "AIPlayer.h"
#include "Tiles/Tile.h"
#include <iostream>

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
    const Tile* bestMoveTarget = nullptr;

    for (const Tile* tile : reachable)
    {
		//first priority: if there's an enemy in range, attack it
        if (tile->hasEntity() && tile->isOccupiedByEnemy(getSide()))
        {
            bestAttackTarget = tile;
			break; // we found an attack target, no need to look for a move target
        }

		//second priority: if there's an empty tile, move to the leftmost one (lowest Q)
        if (!tile->hasEntity() && tile->isPassableFor(monster))
        {
            if (!bestMoveTarget || tile->getQ() < bestMoveTarget->getQ())
            {
                bestMoveTarget = tile;
            }
        }
    }

	// return the best attack target if it exists, otherwise return the best move target
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

