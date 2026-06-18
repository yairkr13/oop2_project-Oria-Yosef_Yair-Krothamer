#include "TurnManager.h"

TurnManager::TurnManager(Player& p1, Player& p2, Board& board)
    : m_player1(p1),
    m_player2(p2),
    m_board(board),
    m_currentPlayer(&p1), // שחקן 1 מתחיל תמיד
    m_turnState(TurnState::ChoosingMonster)
{ }

void TurnManager::handleInput(sf::Vector2f mousePos)
{

    // אם אנחנו בשלב שבו השחקן אמור לבחור מפלצת
    if (m_turnState == TurnState::ChoosingMonster)
    {
        Card* clickedCard = m_currentPlayer->handleCardClick(pos);

        if (clickedCard != nullptr)
        {
            // יש לנו קלף! נשמור אותו ונעבור לשלב הבא
            // m_selectedCard = clickedCard; (תצטרכי להוסיף משתנה כזה ב-Game.h)
            m_turnState = TurnState::MonsterSelected;
        }
        else
        {
            // כאן אפשר לבדוק אם הוא לחץ על מפלצת שכבר קיימת על הלוח
            // m_board.handleMonsterClick(pos);
            //נעשה את זה במחלקה של השחקן
        }
    }
    // אם כבר בחרנו מפלצת, ועכשיו אנחנו מחפשים משבצת על הלוח
    else if (m_turnState == TurnState::MonsterSelected)
    {
        m_board.handleNewMonster(pos, monsterName);
        m_turnState = TurnState::ChoosingTargetTile;
        // כאן נקרא ללוח שיבדוק על איזו משבצת לחצו
        // ושיזמן עליה את ה- m_selectedCard
    }
    else if (m_turnState == TurnState::ChoosingTargetTile)
    {
        // כאן נטפל בלחיצות על מפלצות שכבר קיימות על הלוח
        // לדוגמה, אם השחקן רוצה להזיז או לתקוף עם מפלצת קיימת
        m_board.handleMonsterClick(pos); //אולי פה יבדוק את התקיפה
    }
    //else if(m_turnState == TurnState::Attacking)
    //{
    //    // כאן נטפל בלחיצות שמבצעות התקפה
    //}
    //if() if the click is on the turn end button:
    else if (m_turnState == TurnState::TurnEnded)
    {
        // כאן נטפל במעבר בין תורות
        endTurn();
    }

    //// 1. קודם נעביר את הלחיצה לשחקן הנוכחי.
    //// הפונקציה הזו תחזיר true אם השחקן אכן לחץ על אחד הקלפים שלו.
    //bool handledByPlayer = m_currentPlayer->handleClick(pos);

    //// 2. אם הקליק לא פגע באף קלף של השחקן, נעביר את הלחיצה ללוח.
    //if (!handledByPlayer)
    //{
    //    m_board.handleClick(pos);
    //}
}

void TurnManager::endTurn()
{
    if (m_currentPlayer == &m_player1)
        m_currentPlayer = &m_player2;
    
    else
        m_currentPlayer = &m_player1;

    m_turnState = TurnState::ChoosingMonster;

    // כאן בעתיד נוכל גם לנקות סימונים מהלוח
    // m_board.clearHighlights();
}