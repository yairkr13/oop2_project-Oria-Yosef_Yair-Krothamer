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
    //void beginTurn(Board& board);

    // Called each frame by the game loop. Returns true when the AI turn is complete.
    // Only advances to the next action when no animation is playing.
    //bool advanceTurn(Board& board);

    // Behavioral turn hooks (see Player) - delegate straight to the
    // beginTurn/advanceTurn machinery above so callers never need to know
    // they're dealing with an AIPlayer specifically.
    void onTurnStart(Board& board) override;
    void updateTurn(Board& board) override;
    bool isBusy() const override;

private:

    // ההיוריסטיקה של ה-AI: מתוך כל ה-tiles הנגישים למפלצת (Board::getReachableTiles
    // שהיא שאילתה עובדתית בלבד), *AIPlayer עצמו* בוחר מה עדיף - תקיפה קודמת לתנועה,
    // ובין תנועות - זו שמתקדמת הכי שמאלה. Board לא מעורב בהחלטה בכלל, רק מספק עובדות.
    const Tile* findBestTarget(const Board& board, Monster* monster) const;

    // Same shape as findBestTarget above - Board only ever answers factual
    // queries (getReachableOccupiedTiles), AIPlayer decides. Returns nullptr
    // if the monster's Special isn't usable right now, or no valid target is
    // currently in range (specialAbilityNeedsTarget() must still be checked
    // by the caller first - a no-target Special never needs this at all).
    const Tile* findBestSpecialTarget(const Board& board, Monster* monster) const;

    AITurnPhase m_phase = AITurnPhase::Done;
    int m_currentMonsterIdx = 0;
    int m_safetyCounter = 0;
};