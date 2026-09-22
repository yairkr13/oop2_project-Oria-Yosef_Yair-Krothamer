#pragma once
#include <vector>
#include <memory>
#include <string>
#include <functional>
#include "Monsters/Monster.h"

// �-Factory ��� ����� ����� ���� ����� �� �� ������ ������� ����������.
// �� ����� ���� (Player, Board, Game...) ����� �� �� Monster (����� ������).

class Card; // forward declaration
class MonsterFactory
{
public:
    using Creator = std::function<std::unique_ptr<Monster>(PlayerSide)>; // + PlayerSide

    static std::unique_ptr<Monster> create(const std::string& monsterName, PlayerSide side); // + side

    // ���: createStandardDeck -> vector<unique_ptr<Monster>>
    static std::vector<std::unique_ptr<Card>> createStandardHand(PlayerSide side); // ����� Card, �� Monster

private:
    // One entry per known monster - name, how to build it, and its Card's
    // own cost/texture. The single source of truth both create() (looks
    // one up by name) and createStandardHand() (builds a Card for every
    // one, in this same order) read from, so there's exactly one place
    // that knows "which monsters exist" - not two separately-maintained
    // lists that could silently drift apart.
    struct MonsterDefinition
    {
        std::string name;
        Creator creator;
        int cost;
        std::string cardTextureKey;
    };

    // A std::vector (not the old std::map) deliberately - createStandardHand()'s
    // resulting hand order (Muffintop, Blue, Barzilla, Henrietta, Mozzy) is a
    // chosen gameplay order, not alphabetical; a map would silently
    // re-sort it by key. Only 5 entries, so create()'s linear search here
    // costs nothing worth optimizing.
    static const std::vector<MonsterDefinition>& getCatalog();
};