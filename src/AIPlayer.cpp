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
// אין break! ה-AI ינסה לזמן כל מפלצת שהוא יכול להרשות לעצמו,
// כל עוד יש לו מפתחות ויש משבצות פנויות לזימון.
    for (auto& monsterPtr : m_monsters)
    {
        Monster* monster = monsterPtr.get();

        // בודקים אם המפלצת עדיין "ביד", אם היא חיה, ואם יש לנו עדיין מספיק מפתחות בשבילה
        // (m_keys מתעדכן בכל הצלחה, אז הבדיקה הזו רלוונטית גם למפלצת ה-N בלולאה)
        if (!monster->isOnBoard() && monster->isAlive() && monster->getCost() <= m_keys)
        {
            if (board.AI_SpawnMonster(monster, getSide()))
            {
                reduceKeys(monster->getCost()); // משלמים על המפלצת
                std::cout << "[AI] Spawned monster, keys left: " << m_keys << std::endl;
                // לא עושים break - נמשיך ללולאה הבאה ונבדוק אם יש עוד מפלצת שניתן להרשות
            }
            // אם הזימון נכשל (למשל אין משבצות פנויות בכלל), אין טעם להמשיך לבדוק
            // מפלצות אחרות - כולן ינסו להיכנס לאותו אזור זימון ויכשלו גם הן.
            else
            {
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
            board.performAction(monster, targetTile);
        }
    }

    std::cout << "[AI] Finished turn safely." << std::endl;
}