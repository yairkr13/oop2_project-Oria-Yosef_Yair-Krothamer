#include "RemotePlayer.h"
#include "Board.h"
#include "Card.h"
#include "Monsters/Monster.h"
#include "Tiles/Tile.h"
#include <iostream>

RemotePlayer::RemotePlayer(PlayerSide side, NetworkConnection& connection)
    : Player(side), m_connection(connection)
{
}

void RemotePlayer::onTurnStart(Board& /*board*/)
{
    m_turnFinished = false;
}

void RemotePlayer::updateTurn(Board& board)
{
    if (m_turnFinished) return;
    if (board.isAnimating()) return; // let the previous action's animation resolve first

    // The remote peer's own turn hasn't finished (or hasn't even arrived
    // yet, on a slow connection) - nothing to replay this frame. Not an
    // error: isBusy() below just keeps reporting busy until more arrives.
    if (m_pendingActions.empty()) return;

    GameAction action = m_pendingActions.front();
    m_pendingActions.pop_front();

    if (action.type == GameAction::Type::EndTurn)
    {
        std::cout << "[RemotePlayer] updateTurn - EndTurn popped, m_turnFinished=true" << std::endl; // TEMP DEBUG
        m_turnFinished = true;
        return;
    }

    applyAction(action, board);
}

bool RemotePlayer::isBusy() const
{
    return !m_turnFinished;
}

void RemotePlayer::recordLocalAction(const GameAction& action)
{
    // TEMP DEBUG (see conversation) - remove once the desync is found.
    std::cout << "[RemotePlayer] recordLocalAction type=" << static_cast<int>(action.type)
        << " cardIndex=" << action.cardIndex
        << " source=(" << action.sourceQ << "," << action.sourceRow << ")"
        << " target=(" << action.targetQ << "," << action.targetRow << ")"
        << " hasTarget=" << action.hasTarget << std::endl;

    m_recordedActions.push_back(action);
}

void RemotePlayer::sendRecordedActions()
{
    GameAction endTurn;
    endTurn.type = GameAction::Type::EndTurn;
    m_recordedActions.push_back(endTurn);

    // TEMP DEBUG (see conversation) - remove once the desync is found.
    std::cout << "[RemotePlayer] sendRecordedActions - sending " << m_recordedActions.size() << " actions" << std::endl;

    m_connection.sendMessage(serializeActions(m_recordedActions));
    m_recordedActions.clear();

    // sendMessage() above only queues the bytes (see NetworkConnection) -
    // update() is what actually hands them to the OS socket. Normally the
    // next frame's pollIncoming() would do this anyway, but the one call
    // site that matters most (GameplayState::update()'s isDead() check)
    // transitions away and destroys this connection immediately afterward,
    // with no "next frame" left to flush on - so this can't wait.
    m_connection.update();
}

void RemotePlayer::pollIncoming()
{
    m_connection.update();
    if (m_connection.hasMessage())
        receiveTurnActions(deserializeActions(m_connection.popMessage()));
}

void RemotePlayer::receiveTurnActions(std::vector<GameAction> actions)
{
    // TEMP DEBUG (see conversation) - remove once the desync is found.
    std::cout << "[RemotePlayer] receiveTurnActions - got " << actions.size() << " actions" << std::endl;

    m_pendingActions.assign(actions.begin(), actions.end());
}

void RemotePlayer::applyAction(const GameAction& action, Board& board)
{
    // TEMP DEBUG (see conversation) - remove once the desync is found.
    std::cout << "[RemotePlayer] applyAction type=" << static_cast<int>(action.type)
        << " cardIndex=" << action.cardIndex
        << " source=(" << action.sourceQ << "," << action.sourceRow << ")"
        << " target=(" << action.targetQ << "," << action.targetRow << ")"
        << " hasTarget=" << action.hasTarget << std::endl;

    switch (action.type)
    {
    case GameAction::Type::Spawn:
    {
        if (action.cardIndex < 0 || action.cardIndex >= static_cast<int>(m_hand.size())) return;
        const Tile* tile = board.getTileAt(action.targetQ, action.targetRow);
        if (!tile) return;

        Monster* monster = playCard(m_hand[action.cardIndex].get());
        if (monster)
            board.spawnEntityOnTile(monster, tile);
        break;
    }
    case GameAction::Type::MoveOrAttack:
    {
        const Tile* sourceTile = board.getTileAt(action.sourceQ, action.sourceRow);
        const Tile* targetTile = board.getTileAt(action.targetQ, action.targetRow);
        if (!sourceTile || !targetTile)
        {
            std::cout << "[RemotePlayer] MoveOrAttack ABORTED - sourceTile=" << (sourceTile != nullptr)
                << " targetTile=" << (targetTile != nullptr) << std::endl; // TEMP DEBUG
            return;
        }

        BoardEntity* entity = sourceTile->getMutableEntity();
        std::cout << "[RemotePlayer] MoveOrAttack - entity on sourceTile=" << (entity != nullptr) << std::endl; // TEMP DEBUG
        if (entity)
            board.performAction(entity, targetTile);
        break;
    }
    case GameAction::Type::Special:
    {
        if (action.cardIndex < 0 || action.cardIndex >= static_cast<int>(m_hand.size())) return;
        Monster* monster = m_hand[action.cardIndex]->getMutableLinkedMonster();
        if (!monster || !monster->canUseSpecialAbilityNow()) return;

        if (!action.hasTarget)
        {
            monster->useSpecialAbility(board);
            break;
        }

        const Tile* targetTile = board.getTileAt(action.targetQ, action.targetRow);
        BoardEntity* candidate = targetTile ? targetTile->getMutableEntity() : nullptr;
        if (candidate && monster->isValidSpecialTarget(*candidate))
            monster->useSpecialAbility(board, candidate);
        break;
    }
    case GameAction::Type::EndTurn:
        break; // handled in updateTurn before reaching here
    }
}
