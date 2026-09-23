#pragma once
#include "Player.h"
#include "GameAction.h"
#include "NetworkConnection.h"
#include <deque>
#include <vector>

class Board;

// A Player whose turn is driven by another computer over the network,
// instead of local mouse/keyboard input (see GameplayState's handle*
// methods) or an AI heuristic (see AIPlayer) - the third case
// Player::onTurnStart/updateTurn/isBusy already anticipates.
//
// Owns BOTH directions of this match's network traffic, not just the
// "replay the opponent's turn" half its name suggests, and owns the
// NetworkConnection itself - it's the one class that actually needs it, so
// GameplayState never touches a GameAction, a byte, or the connection
// directly, only ever recordLocalAction()/sendRecordedActions()/pollIncoming().
class RemotePlayer : public Player
{
public:
    // `connection` must already be connected (see the lobby State) and
    // must outlive this RemotePlayer - owned by whoever also owns the
    // GameplayState this player belongs to.
    RemotePlayer(PlayerSide side, NetworkConnection& connection);

    void onTurnStart(Board& board) override;
    void updateTurn(Board& board) override;
    bool isBusy() const override;

    // Called by GameplayState as the local human (the OTHER Player in this
    // match) acts, only while playing PlayerVsRemote.
    void recordLocalAction(const GameAction& action);

    // Called once, when the local human's own turn ends: appends the
    // EndTurn marker, serializes everything recorded since the last call,
    // and sends it - then clears the recorded list for next time.
    void sendRecordedActions();

    // Called once per frame (see GameplayState::update) regardless of
    // whose turn it locally is, so the connection is always serviced -
    // pumps the socket and, if a full message has arrived, feeds it into
    // this player's own pending-actions queue.
    void pollIncoming();

private:
    void receiveTurnActions(std::vector<GameAction> actions);
    // Replays one GameAction through the same Board/Monster calls a local
    // click would make (Board::spawnEntityOnTile/performAction,
    // Monster::useSpecialAbility) - never a parallel "apply the result"
    // mechanism, so this player's turn always plays out through the exact
    // same rules as everyone else's.
    void applyAction(const GameAction& action, Board& board);

    NetworkConnection& m_connection;
    std::vector<GameAction> m_recordedActions; // outgoing: the local human's actions, collected to send
    std::deque<GameAction> m_pendingActions;   // incoming: this player's own actions, waiting to be replayed
    bool m_turnFinished = true;
};
