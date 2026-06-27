#pragma once
#include "Player.h"
#include "Board.h"

class AIPlayer : public Player
{
public:
    // קונסטרקטור שמקבל את הצד (בדרך כלל Right) ושם
    AIPlayer(PlayerSide side);

    // מתודת הליבה של המחשב - תורץ בכל פעם שהתור של ה-AI מתחיל
    void makeMove(Board& board);
};