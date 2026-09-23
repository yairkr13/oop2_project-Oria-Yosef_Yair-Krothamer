#include "GameAction.h"
#include <cstring>

namespace
{
    // Every field is written as a plain 4-byte native int (no htonl/ntohl -
    // unlike NetworkConnection's own length prefix, this never needs to be
    // understood by anything other than another copy of this same project,
    // always compiled for the same Windows x64 target, so there's no real
    // byte-order mismatch to guard against here, and it keeps this file
    // free of any networking-specific header).
    void writeInt(std::vector<std::uint8_t>& out, int value)
    {
        std::uint8_t bytes[4];
        std::memcpy(bytes, &value, 4);
        out.insert(out.end(), bytes, bytes + 4);
    }

    int readInt(const std::uint8_t* data)
    {
        int value;
        std::memcpy(&value, data, 4);
        return value;
    }
}

std::vector<std::uint8_t> serializeActions(const std::vector<GameAction>& actions)
{
    std::vector<std::uint8_t> out;
    writeInt(out, static_cast<int>(actions.size()));

    for (const GameAction& action : actions)
    {
        writeInt(out, static_cast<int>(action.type));
        writeInt(out, action.cardIndex);
        writeInt(out, action.sourceQ);
        writeInt(out, action.sourceRow);
        out.push_back(action.hasTarget ? 1 : 0);
        writeInt(out, action.targetQ);
        writeInt(out, action.targetRow);
    }
    return out;
}

std::vector<GameAction> deserializeActions(const std::vector<std::uint8_t>& bytes)
{
    std::vector<GameAction> actions;

    // 4 (count) + at least one full 25-byte record if count > 0 - anything
    // shorter is a malformed/truncated message, not a valid empty list.
    if (bytes.size() < 4) return actions;

    int count = readInt(bytes.data());
    constexpr std::size_t RECORD_SIZE = 4 + 4 + 4 + 4 + 1 + 4 + 4; // 25 bytes
    std::size_t offset = 4;

    for (int i = 0; i < count; ++i)
    {
        if (offset + RECORD_SIZE > bytes.size()) break; // truncated - stop at whatever's actually there

        GameAction action;
        action.type = static_cast<GameAction::Type>(readInt(&bytes[offset]));
        action.cardIndex = readInt(&bytes[offset + 4]);
        action.sourceQ = readInt(&bytes[offset + 8]);
        action.sourceRow = readInt(&bytes[offset + 12]);
        action.hasTarget = bytes[offset + 16] != 0;
        action.targetQ = readInt(&bytes[offset + 17]);
        action.targetRow = readInt(&bytes[offset + 21]);
        actions.push_back(action);

        offset += RECORD_SIZE;
    }
    return actions;
}
