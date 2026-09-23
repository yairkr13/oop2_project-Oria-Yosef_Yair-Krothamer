#pragma once
#include <vector>
#include <memory>
#include <string>
#include <functional>
#include "Monsters/Monster.h"

// Builds concrete Monster instances (and their Card counterparts) by name,
// so callers (Player, Board, Game...) never need to know about each
// concrete Monster subclass directly.

class Card; // forward declaration
class MonsterFactory
{
public:
    using Creator = std::function<std::unique_ptr<Monster>(PlayerSide)>;

    static std::unique_ptr<Monster> create(const std::string& monsterName, PlayerSide side);

    static std::vector<std::unique_ptr<Card>> createStandardHand(PlayerSide side);

private:
    // One entry per known monster: name, how to build it, and its Card's
    // cost/texture. Single source of truth for both create() and
    // createStandardHand().
    struct MonsterDefinition
    {
        std::string name;
        Creator creator;
        int cost;
        std::string cardTextureKey;
    };

    // A vector, not a map: createStandardHand()'s hand order is a chosen
    // gameplay order, not alphabetical - a map would silently re-sort it.
    static const std::vector<MonsterDefinition>& getCatalog();
};
