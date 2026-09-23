#pragma once
#include <cstdint>
#include <vector>

// One player-driven event during a turn (spawn, move/attack, Special,
// EndTurn), recorded as plain data so it can be sent to a remote peer and
// replayed there through the same Board/Player calls a local click makes
// (see RemotePlayer). Uses board coordinates (q, row), never screen
// positions - those mean a different thing on each computer.
struct GameAction
{
    enum class Type { Spawn, MoveOrAttack, Special, EndTurn };

    Type type;

    // Spawn: which card in the acting player's hand (by index).
    // Special: which card was clicked to trigger it - Specials are always
    // triggered by clicking the played CARD, never the on-board monster, so
    // this (not sourceQ/sourceRow) identifies the caster.
    int cardIndex = -1;

    // MoveOrAttack only: which on-board entity was selected, by position.
    int sourceQ = 0;
    int sourceRow = 0;

    // Spawn: the tile clicked to place the new monster.
    // MoveOrAttack: the tile clicked to move/attack into.
    // Special: the tile clicked as the ability's target - only when hasTarget is true.
    bool hasTarget = false;
    int targetQ = 0;
    int targetRow = 0;
};

// A whole turn's worth of actions as bytes ready for NetworkConnection::sendMessage.
std::vector<std::uint8_t> serializeActions(const std::vector<GameAction>& actions);
std::vector<GameAction> deserializeActions(const std::vector<std::uint8_t>& bytes);
