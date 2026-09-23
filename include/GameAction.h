#pragma once
#include <cstdint>
#include <vector>

// One player-driven thing that happened during a turn - spawning a card,
// moving/attacking, using a Special, or ending the turn - recorded as plain
// data (not a polymorphic Command hierarchy: there are only 4 kinds, and
// none of them differ enough in shape to need their own class) so it can be
// sent to a remote peer and replayed there through the exact same Board/
// Player calls GameplayState already makes for a local click - see
// RemotePlayer.
//
// Board coordinates (never screen positions - a screen position means a
// different thing on each computer, depending on that computer's own
// window; (q, row) means the same thing on both).
struct GameAction
{
    enum class Type { Spawn, MoveOrAttack, Special, EndTurn };

    Type type;

    // Spawn: which card in the acting player's hand (by index).
    // Special: which card was clicked to trigger it - specials are always
    // triggered by clicking the already-played CARD (see
    // GameplayState::handleSpecialAbilityClick), never by clicking the
    // on-board monster directly, so this - not sourceQ/sourceRow - is how
    // a Special identifies its caster.
    int cardIndex = -1;

    // MoveOrAttack only: which on-board entity was selected, by position -
    // this one genuinely is board-click-driven (see
    // GameplayState::handleBoardClick's m_selectedEntity).
    int sourceQ = 0;
    int sourceRow = 0;

    // Spawn: the tile that was clicked to place the new monster.
    // MoveOrAttack: the tile that was clicked to move/attack into.
    // Special: the tile that was clicked as the ability's target - only
    // when hasTarget is true. (0, 0) is itself a real, valid board
    // coordinate, so it can't double as a sentinel for "no target" (a
    // no-target Special, Spawn/MoveOrAttack's own hasTarget is always
    // true, and EndTurn, which uses neither).
    bool hasTarget = false;
    int targetQ = 0;
    int targetRow = 0;
};

// A whole turn's worth of actions, as bytes ready for
// NetworkConnection::sendMessage - a fixed-size record per action (no
// variable-length fields in GameAction, so no per-field length prefixes
// are needed), preceded by how many there are.
std::vector<std::uint8_t> serializeActions(const std::vector<GameAction>& actions);
std::vector<GameAction> deserializeActions(const std::vector<std::uint8_t>& bytes);
