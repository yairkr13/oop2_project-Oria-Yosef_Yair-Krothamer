#include "AIPlayer.h"
#include <iostream>

AIPlayer::AIPlayer(PlayerSide side)
    : Player(side)
{
}

void AIPlayer::makeMove(Board& board)
{
    std::cout << "[AI] Starting turn..." << std::endl;

    // ============================================
    // שלב 1: זימון מפלצות (Spawning)
    // ============================================
    for (auto& monsterPtr : m_monsters)
    {
        Monster* monster = monsterPtr.get();

        // בודקים אם המפלצת עדיין "ביד", אם היא חיה, ואם יש לנו מספיק מפתחות בשבילה
        if (!monster->isOnBoard() && monster->isAlive() && monster->getCost() <= m_keys)
        {
            if (board.AI_SpawnMonster(monster, getSide()))
            {
                reduceKeys(monster->getCost()); // משלמים על המפלצת
                std::cout << "[AI] Spawned monster: "  << std::endl;

                // אפשרות: לעשות break פה כדי לזמן רק מפלצת אחת בתור, 
                // או לתת לו לזמן כמה שהוא רוצה אם יש לו מפתחות. נשים break לבנתיים.
                break;
            }
        }
    }

    // ============================================
    // שלב 2: תנועה ותקיפה (Action Phase)
    // ============================================
    for (auto& monsterPtr : m_monsters)
    {
        Monster* monster = monsterPtr.get();

        // חובה! מבצעים פעולות רק עבור מפלצת שהיא חיה וכבר נמצאת על הלוח!
        if (!monster || !monster->isAlive() || !monster->isOnBoard()) continue;

        int safetyCounter = 0; // למנוע לולאה אינסופית

        while (monster->getActionsLeft() > 0 && monster->isAlive() && safetyCounter < 5)
        {
            safetyCounter++;

            // שואלים את הלוח מה המשבצת הכי טובה למפלצת הזו כרגע
            Tile* targetTile = board.AI_FindBestTargetForMonster(monster);

            if (!targetTile) {
                break; // אין שום מהלך חוקי פנוי, נעבור למפלצת הבאה
            }

            std::cout << "[AI] Moving/Attacking with: " <<  std::endl;

            // הלוח מבצע את הפעולה ומוריד את נקודת הפעולה למפלצת
            board.AI_ExecuteAction(monster, targetTile);
        }
    }

    std::cout << "[AI] Finished turn safely." << std::endl;
}