#include "AIPlayer.h"
#include "Tiles/Tile.h"
#include <iostream>

AIPlayer::AIPlayer(PlayerSide side)
    : Player(side)
{
}
//
//void AIPlayer::beginTurn(Board& board)
//{
//    std::cout << "[AI] Starting turn..." << std::endl;
//
//    // Phase 1: Spawn all affordable monsters immediately (spawning is instant, no animation)
//    for (auto& monsterPtr : m_monsters)
//    {
//        Monster* monster = monsterPtr.get();
//
//        if (!monster->isOnBoard() && monster->isAlive() && monster->getCost() <= m_keys)
//        {
//            if (board.AI_SpawnMonster(monster, getSide()))
//            {
//                reduceKeys(monster->getCost());
//                std::cout << "[AI] Spawned monster, keys left: " << m_keys << std::endl;
//            }
//            else
//            {
//                break;
//            }
//        }
//    }
//
//    // Prepare for action phase: start from first monster
//    m_phase = AITurnPhase::Acting;
//    m_currentMonsterIdx = 0;
//    m_safetyCounter = 0;
//}
//
//bool AIPlayer::advanceTurn(Board& board)
//{
//    if (m_phase == AITurnPhase::Done)
//        return true;
//
//    // Wait for any active animation to finish before executing next action
//    if (board.isAnimating())
//        return false;
//
//    // Find the current monster to act with
//    while (m_currentMonsterIdx < static_cast<int>(m_monsters.size()))
//    {
//        Monster* monster = m_monsters[m_currentMonsterIdx].get();
//
//        // Skip dead/off-board monsters
//        if (!monster || !monster->isAlive() || !monster->isOnBoard())
//        {
//            m_currentMonsterIdx++;
//            m_safetyCounter = 0;
//            continue;
//        }
//        // Check if this monster still has actions
//        if (monster->getActionsLeft() <= 0 || m_safetyCounter >= 5)
//        {
//            m_currentMonsterIdx++;
//            m_safetyCounter = 0;
//            continue;
//        }
//
//        // Find best target for this monster
//        Tile* targetTile = findBestTarget(board, monster);
//
//        if (!targetTile)
//        {
//            // No valid target, move to next monster
//            m_currentMonsterIdx++;
//            m_safetyCounter = 0;
//            continue;
//        }
//
//        // Execute this single action (will trigger walkTo animation for moves)
//        m_safetyCounter++;
//        std::cout << "[AI] Performing action with monster at (" << monster->getQ() << "," << monster->getRow() << ")" << std::endl;
//        board.performAction(monster, targetTile);
//
//        // Return false � we executed one action, now wait for animation to finish
//        return false;
//    }
//
//    // All monsters processed � AI turn is done
//    m_phase = AITurnPhase::Done;
//    std::cout << "[AI] Finished turn." << std::endl;
//    return true;
//}

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
            //if (!bestMoveTarget)
            //{
            //    bestMoveTarget = tile;
            //}
            //else
            //{
            //    // Player1 מתקדם ימינה (Q גדל), Player2 מתקדם שמאלה (Q קטן)
            //    bool isBetter = (getSide() == PlayerSide::Player1)
            //        ? (tile->getQ() > bestMoveTarget->getQ())
            //        : (tile->getQ() < bestMoveTarget->getQ());

            //    if (isBetter)
            //        bestMoveTarget = tile;
            //}
        }
    }

    // נחזיר קודם כל תקיפה, ואם אין - תנועה
    return bestAttackTarget ? bestAttackTarget : bestMoveTarget;
}
//void AIPlayer::onTurnStart(Board& board)
//{
//    for (auto& cardPtr : m_hand) // range-based, בטוח כי אין erase
//    {
//        Card* card = cardPtr.get();
//        if (card->isPlayed() || card->getCost() > m_keys)
//            continue;
//
//        // Board חושף רק עובדה: "אילו tiles פנויים לצד הזה" - לא מכיר Card בכלל
//        std::vector<Tile*> candidates = board.getSpawnableTiles(getSide());
//        if (candidates.empty())
//            break;
//
//        std::uniform_int_distribution<size_t> dist(0, candidates.size() - 1);
//        Tile* chosenTile = candidates[dist(Board::rng())];
//
//        Monster* monster = playCard(card); // AIPlayer עצמו - לא Board
//        if (monster)
//        {
//            //monster->spawnOnBoard(chosenTile->getQ(), chosenTile->getRow(), chosenTile->getScreenPos());
//            monster->spawnOnBoard(chosenTile->getQ(), chosenTile->getRow(), board.tileToScreen(chosenTile->getQ(), chosenTile->getRow()));
//            chosenTile->setEntity(monster);
//        }
//    }
//
//    m_phase = AITurnPhase::Acting;
//    m_currentMonsterIdx = 0;
//    m_safetyCounter = 0;
//}
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
    //for (auto& monsterPtr : m_monsters)
    //{
    //    Monster* monster = monsterPtr.get();
    //    if (monster->isOnBoard() || !monster->isAlive() || monster->getCost() > m_keys)
    //        continue;

    //    std::vector<Tile*> candidates = board.getSpawnableTiles(monster, getSide());
    //    if (candidates.empty())
    //        break; // אין יותר מקום לזמן - אין טעם להמשיך לבדוק מפלצות נוספות

    //    std::uniform_int_distribution<size_t> dist(0, candidates.size() - 1);
    //    Tile* chosenTile = candidates[dist(Board::rng())];

    //    if (board.spawnMonsterOnTile(monster, chosenTile))
    //    {
    //        reduceKeys(monster->getCost());
    //    }
    //    
    //    
    //}

    // שלב 2: הכנה לשלב הפעולה - נתחיל מהמפלצת הראשונה
    m_phase = AITurnPhase::Acting;
    m_currentMonsterIdx = 0;
    m_safetyCounter = 0;

   // beginTurn(board);
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

        // Return false � we executed one action, now wait for animation to finish
        return ;
    }

    // All monsters processed � AI turn is done
    m_phase = AITurnPhase::Done;
    /*std::cout << "[AI] Finished turn." << std::endl;
    return ;
    advanceTurn(board);*/
}

bool AIPlayer::isBusy() const
{
    return m_phase != AITurnPhase::Done;
}