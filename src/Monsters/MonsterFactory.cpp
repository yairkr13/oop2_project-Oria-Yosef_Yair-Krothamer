#include "Monsters/MonsterFactory.h"
#include "Card.h"
#include <iostream>
#include "Monsters/Muffintop.h"
#include "Monsters/Blue.h"
#include "Monsters/Barzilla.h"
#include "Monsters/Henrietta.h"
#include "Monsters/Mozzy.h"

const std::vector<MonsterFactory::MonsterDefinition>& MonsterFactory::getCatalog()
{
    static const std::vector<MonsterDefinition> catalog = {
        { "Muffintop", [](PlayerSide side) { return std::make_unique<Muffintop>(side); }, 2, "muffintop" },
        { "Blue",      [](PlayerSide side) { return std::make_unique<Blue>(side); },      3, "blue" },
        { "Barzilla",  [](PlayerSide side) { return std::make_unique<Barzilla>(side); },  4, "barzilla" },
        { "Henrietta", [](PlayerSide side) { return std::make_unique<Henrietta>(side); }, 3, "henrietta" },
        { "Mozzy",     [](PlayerSide side) { return std::make_unique<Mozzy>(side); },     2, "mozzy" },
    };
    return catalog;
}

std::unique_ptr<Monster> MonsterFactory::create(const std::string& monsterName, PlayerSide side)
{
    for (const auto& def : getCatalog())
    {
        if (def.name == monsterName)
            return def.creator(side);
    }

    std::cout << "[ERROR] MonsterFactory: Monster name '" << monsterName << "' not found in registry!\n";
    return nullptr;
}

std::vector<std::unique_ptr<Card>> MonsterFactory::createStandardHand(PlayerSide side)
{
    std::vector<std::unique_ptr<Card>> hand;
    for (const auto& def : getCatalog())
        hand.push_back(std::make_unique<Card>(def.name, def.cost, def.cardTextureKey, side));
    return hand;
}