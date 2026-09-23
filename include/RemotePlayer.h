#pragma once
#include "Player.h"
#include "GameAction.h"
#include "NetworkConnection.h"
#include <deque>
#include <vector>

class Board;

// A Player whose turn is driven by another computer over the network,
// instead of local input (GameplayState) or an AI heuristic (AIPlayer).
// Owns both directions of this match's network traffic and the
// NetworkConnection itself - GameplayState only ever calls
// recordLocalAction()/sendRecordedActions()/pollIncoming().
class RemotePlayer : public Player
{
public:
    // connection must already be connected and must outlive this
    // RemotePlayer - owned by whoever also owns the GameplayState.
    RemotePlayer(PlayerSide side, NetworkConnection& connection);

    void onTurnStart(Board& board) override;
    void updateTurn(Board& board) override;
    bool isBusy() const override;

    // Called by GameplayState as the local human acts, only while playing PlayerVsRemote.
    void recordLocalAction(const GameAction& action);

    // Called once the local human's turn ends: appends EndTurn, serializes
    // everything recorded since last call, sends it, then clears the list.
    void sendRecordedActions();

    // Called once per frame regardless of whose turn it locally is, so the
    // connection is always serviced.
    void pollIncoming();

private:
    void receiveTurnActions(std::vector<GameAction> actions);
    // Replays one GameAction through the same Board/Monster calls a local
    // click would make, never a parallel "apply the result" mechanism.
    void applyAction(const GameAction& action, Board& board);

    NetworkConnection& m_connection;
    std::vector<GameAction> m_recordedActions; // outgoing: the local human's actions, collected to send
    std::deque<GameAction> m_pendingActions;   // incoming: this player's own actions, waiting to be replayed
    bool m_turnFinished = true;
};
