#pragma once
#include "Player.h"
#include "Board.h"

enum class AITurnPhase { Spawning, Acting, Done };

// Player driven by a simple heuristic AI: spawns whatever it can afford,
// then for each on-board monster uses its Special if a target is in range,
// otherwise attacks the best enemy in range or advances toward the enemy's Heart.
class AIPlayer : public Player
{
public:
    AIPlayer(PlayerSide side);

    // Behavioral turn hooks (see Player) - delegate to the phase machine below.
    void onTurnStart(Board& board) override;
    void updateTurn(Board& board) override;
    bool isBusy() const override;

private:
    // Picks the best reachable tile for this monster: attack outranks
    // movement, and among moves the one advancing closest to the enemy's back row.
    const Tile* findBestTarget(const Board& board, const Monster* monster) const;

    // Same shape as findBestTarget - Board only answers factual queries,
    // AIPlayer decides. Returns nullptr if no valid Special target is in range.
    const Tile* findBestSpecialTarget(const Board& board, const Monster* monster) const;

    AITurnPhase m_phase = AITurnPhase::Done;
    int m_currentMonsterIdx = 0;
    int m_safetyCounter = 0;
};
