#include "Monsters/MonsterFactory.h"
#include "Card.h"
#include <iostream>
#include "Monsters/Muffintop.h"
#include "Monsters/Blue.h"
#include "Monsters/Barzilla.h"
#include "Monsters/Henrietta.h"
#include "Monsters/Mozzy.h"

std::map<std::string, MonsterFactory::Creator>& MonsterFactory::getRegistry()
{
    static std::map<std::string, Creator> registry = []() {
        std::map<std::string, Creator> reg;
        reg["Muffintop"] = [](PlayerSide side) { return std::make_unique<Muffintop>(side); };
        reg["Blue"] = [](PlayerSide side) { return std::make_unique<Blue>(side); };
        reg["Barzilla"] = [](PlayerSide side) { return std::make_unique<Barzilla>(side); };
        reg["Henrietta"] = [](PlayerSide side) { return std::make_unique<Henrietta>(side); };
        reg["Mozzy"] = [](PlayerSide side) { return std::make_unique<Mozzy>(side); };
        return reg;
        }();

    return registry;
}

void MonsterFactory::registerMonster(const std::string& name, Creator creator)
{
    getRegistry()[name] = std::move(creator);
}


std::unique_ptr<Monster> MonsterFactory::create(const std::string& monsterName, PlayerSide side)
{
    auto& registry = getRegistry();
    auto it = registry.find(monsterName);
    if (it == registry.end())
    {
        std::cout << "[ERROR] MonsterFactory: Monster name '" << monsterName << "' not found in registry!\n";
        return nullptr;
    }
    return it->second(side);
}

std::vector<std::unique_ptr<Card>> MonsterFactory::createStandardHand(PlayerSide side)
{
    std::vector<std::unique_ptr<Card>> hand;
    hand.push_back(std::make_unique<Card>("Muffintop", 2, "muffintop", side));
    hand.push_back(std::make_unique<Card>("Blue", 3, "blue", side));
    hand.push_back(std::make_unique<Card>("Barzilla", 4, "barzilla", side));
    hand.push_back(std::make_unique<Card>("Henrietta", 3, "henrietta", side));
    hand.push_back(std::make_unique<Card>("Mozzy", 2, "mozzy", side));
    return hand;
}