#include "MonsterFactory.h"

// אלו ה-includes היחידים בכל הפרויקט שמכירים את המחלקות הקונקרטיות.
// אם תרצה להוסיף מפלצת חדשה - זה המקום היחיד שצריך לגעת בו (חוץ מליצור את הקובץ עצמו).
#include "Monsters/Muffintop.h"
#include "Monsters/Blue.h"
#include "Monsters/Barzilla.h"
#include "Monsters/Henrietta.h"
#include "Monsters/Mozzy.h"

std::map<std::string, MonsterFactory::Creator>& MonsterFactory::getRegistry()
{
    // static בתוך פונקציה -> מאותחל פעם אחת, ללא תלות בסדר אתחול גלובלי
    static std::map<std::string, Creator> registry;
    return registry;
}

void MonsterFactory::registerMonster(const std::string& name, Creator creator)
{
    getRegistry()[name] = std::move(creator);
}

std::unique_ptr<Monster> MonsterFactory::create(const std::string& monsterName)
{
    auto& registry = getRegistry();
    auto it = registry.find(monsterName);
    if (it == registry.end())
        return nullptr; // שם לא מוכר - לא קורסים, מחזירים nullptr

    return it->second();
}

std::vector<std::unique_ptr<Monster>> MonsterFactory::createStandardDeck(PlayerSide side)
{
    std::vector<std::unique_ptr<Monster>> deck;
    deck.push_back(std::make_unique<Muffintop>(side));
    deck.push_back(std::make_unique<Blue>(side));
    deck.push_back(std::make_unique<Barzilla>(side));
    deck.push_back(std::make_unique<Henrietta>(side));
    deck.push_back(std::make_unique<Mozzy>(side));
    return deck;
}