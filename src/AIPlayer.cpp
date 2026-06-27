//#include "AIPlayer.h"
//
//AIPlayer::AIPlayer(PlayerSide side)
//    : Player(side)
//{
//}
//
//void AIPlayer::makeMove(Board& board)
//{
//    // רצים על המפלצות של השחקן (מתוך ה-vector שלו)
//    for (auto& monsterPtr : m_monsters)
//    {
//        Monster* monster = monsterPtr.get();
//        if (!monster || !monster->isAlive()) continue;
//
//        while (monster->getActionsLeft() > 0 && monster->isAlive())
//        {
//            // שואלים את הלוח: "מה היעד הכי טוב למפלצת הזו כרגע?"
//            Tile* targetTile = board.AI_FindBestTargetForMonster(monster);
//
//            if (!targetTile) break; // אין שום מהלך חוקי, נעבור למפלצת הבאה
//
//            // מבצעים את הפעולה דרך פונקציית handleClick הקיימת שלך!
//            // אנחנו פשוט "ממזגים" לה את הקורדינטות הפיזיות של משבצת היעד
//            sf::Vector2f targetScreenPos = board.tileToScreen(targetTile->getQ(), targetTile->getRow());
//
//            // ה-AI פשוט עושה "קליק וירטואלי" על המשבצת שהלוח בחר!
//            board.handleClick(targetScreenPos, this->getSide());
//        }
//    }
//}