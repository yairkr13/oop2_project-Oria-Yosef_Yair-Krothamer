#include "TurnManager.h"
#include "Player.h"
#include "Board.h"

TurnManager::TurnManager(Player& player1, Player& player2, Board& board)
    : m_player1(player1)
    , m_player2(player2)
    , m_board(board)
    , m_currentPlayer(&player1)
{
    m_currentPlayer->onTurnStart(m_board);
}

void TurnManager::update()
{
    bool wasBusy = m_currentPlayer->isBusy();
    m_currentPlayer->updateTurn(m_board);

    if (wasBusy && !m_currentPlayer->isBusy())
        switchToNextPlayer();
}

void TurnManager::requestEndTurn()
{
    if (!canAcceptInput())
        return;

    switchToNextPlayer();
}

bool TurnManager::canAcceptInput() const
{
    return !m_currentPlayer->isBusy();
}

void TurnManager::switchToNextPlayer()
{
    m_currentPlayer->endTurn();
    m_board.updateTileEffects();

    m_currentPlayer = (m_currentPlayer == &m_player1) ? &m_player2 : &m_player1;
    m_currentPlayer->onTurnStart(m_board);
}
