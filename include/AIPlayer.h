#pragma once
#include "Player.h"
#include "Board.h"
//CHECK
enum class AITurnPhase { Spawning, Acting, Done };

class AIPlayer : public Player
{
public:

    // ���������� ����� �� ��� (���� ��� Right) ���
    AIPlayer(PlayerSide side);

    // ����� ����� �� ����� - ���� ��� ��� ����� �� �-AI �����
    // Called once at start of AI turn to begin the animated turn sequence
    void beginTurn(Board& board);

    // Called each frame by the game loop. Returns true when the AI turn is complete.
    // Only advances to the next action when no animation is playing.
    bool advanceTurn(Board& board);

    // Behavioral turn hooks (see Player) - delegate straight to the
    // beginTurn/advanceTurn machinery above so callers never need to know
    // they're dealing with an AIPlayer specifically.
    void onTurnStart(Board& board) override;
    void updateTurn(Board& board) override;
    bool isBusy() const override;

private:

    AITurnPhase m_phase = AITurnPhase::Done;
    int m_currentMonsterIdx = 0;
    int m_safetyCounter = 0;
};

