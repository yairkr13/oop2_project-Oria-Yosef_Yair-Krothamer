#pragma once
#include "Player.h"
#include "Board.h"

enum class AITurnPhase { Spawning, Acting, Done };

class AIPlayer : public Player
{
public:

    // קונסטרקטור שמקבל את הצד (בדרך כלל Right) ושם
    AIPlayer(PlayerSide side);

    // מתודת הליבה של המחשב - תורץ בכל פעם שהתור של ה-AI מתחיל
    // Called once at start of AI turn to begin the animated turn sequence
    void beginTurn(Board& board);

    // Called each frame by the game loop. Returns true when the AI turn is complete.
    // Only advances to the next action when no animation is playing.
    bool advanceTurn(Board& board);


private:

    AITurnPhase m_phase = AITurnPhase::Done;
    int m_currentMonsterIdx = 0;
    int m_safetyCounter = 0;
};