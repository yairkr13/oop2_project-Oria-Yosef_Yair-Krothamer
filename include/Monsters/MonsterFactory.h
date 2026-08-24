#pragma once
#include <vector>
#include <memory>
#include <string>
#include <functional>
#include <map>
#include "Monsters/Monster.h"

// ה-Factory הוא המקום היחיד בקוד שמכיר את כל מחלקות המפלצות הקונקרטיות.
// כל מחלקה אחרת (Player, Board, Game...) מכירה רק את Monster (הבסיס המופשט).

class Card; // forward declaration
class MonsterFactory
{
public:
    using Creator = std::function<std::unique_ptr<Monster>(PlayerSide)>; // + PlayerSide

    static void registerMonster(const std::string& name, Creator creator);
    static std::unique_ptr<Monster> create(const std::string& monsterName, PlayerSide side); // + side

    // היה: createStandardDeck -> vector<unique_ptr<Monster>>
    static std::vector<std::unique_ptr<Card>> createStandardHand(PlayerSide side); // מחזיר Card, לא Monster

private:
    static std::map<std::string, Creator>& getRegistry();
};