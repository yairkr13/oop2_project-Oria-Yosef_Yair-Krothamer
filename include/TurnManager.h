#pragma once

class Player;
class Board;

// Orchestrates whose turn it is and drives it forward. Owns no game
// objects - Player/Board are owned by GameplayState and only referenced
// here, matching how AIPlayer::beginTurn/advanceTurn already take Board&
// rather than storing it.
class TurnManager
{
public:
    TurnManager(Player& player1, Player& player2, Board& board);

    // Call every frame. Advances the current player's turn and
    // automatically switches to the other player once an automated turn
    // (see Player::isBusy()) finishes on its own.
    void update();

    // Call when the current player asks to end their turn (e.g. Space).
    // No-op if the current player is busy (e.g. mid AI turn).
    void requestEndTurn();

    Player& getCurrentPlayer() const { return *m_currentPlayer; }

    // The single place that answers whether gameplay input should currently
    // be accepted - callers never need to know the underlying rule.
    bool canAcceptInput() const;

private:
    void switchToNextPlayer();

    Player& m_player1;
    Player& m_player2;
    Board& m_board;
    Player* m_currentPlayer;
};
