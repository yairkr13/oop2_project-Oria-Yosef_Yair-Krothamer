#pragma once
#include <vector>
#include <memory>
#include <string>
#include <functional>
#include <map>
#include "Monsters/Monster.h"

// ה-Factory הוא המקום היחיד בקוד שמכיר את כל מחלקות המפלצות הקונקרטיות.
// כל מחלקה אחרת (Player, Board, Game...) מכירה רק את Monster (הבסיס המופשט).
class MonsterFactory
{
public:
    // יוצר עותק חדש של כל סוגי המפלצות הסטנדרטיים (זה מה ש-Player צריך בבנאי שלו)

    // יצירת מפלצת בודדת לפי שם - שימושי ל-Save/Load, AI, או הרחבות עתידיות
    static std::unique_ptr<Monster> create(const std::string& monsterName);

    // מאפשר לרשום סוג מפלצת חדש "מבחוץ" בלי לערוך את ה-Factory עצמו
    using Creator = std::function<std::unique_ptr<Monster>()>;
    static void registerMonster(const std::string& name, Creator creator);
    static std::vector<std::unique_ptr<Monster>> createStandardDeck(PlayerSide side);
private:
    static std::map<std::string, Creator>& getRegistry();
};