#include "Player.h"
#include "MonsterFactory.h"
#include "Constants.h"
#include "Tile.h"
#include "AssetsManager.h"

Player::Player(PlayerSide side)
	: m_heart(std::make_unique<Heart>(side, -1, -1, sf::Vector2f(0.f, 0.f))), // יוצרים את הלב עם מיקום זמני
    /*m_heart(std::make_unique<Heart>()),*/ m_keys(3),m_maxKeys(12), m_side(side)
{
    m_hand = MonsterFactory::createStandardHand(side);
	// Initialize cards
    //m_monsters = MonsterFactory::createStandardDeck(side);

    //m_monsters.push_back(std::make_unique<Muffintop>());
    //m_monsters.push_back(std::make_unique<Blue>());
    //m_monsters.push_back(std::make_unique<Barzilla>());
    //m_monsters.push_back(std::make_unique<Henrietta>());
    //m_monsters.push_back(std::make_unique<Mozzy>());

    // Set side on all monsters
    /*for (auto& monster : m_monsters)
        monster->setSide(m_side);*/
}
/*
void Player::handleClick(sf::Vector2f pos)
{
	for (auto& monster : m_monsters)
	{
		if (!monster->isOnBoard() && monster->contains(pos, screenPos in hand))
		{
			m_selected = monster.get();
			return;
		}
	}
}*/
//void Player::draw(sf::RenderWindow& window, bool alignRight, Monster* selectedFromHand) const
//{
//    //drawKeys(window, alignRight);
//    drawHand(window, alignRight, selectedFromHand);
//}
void Player::draw(sf::RenderWindow& window, bool alignRight, Card* selectedFromHand) const
{
    drawHand(window, alignRight, selectedFromHand);
}

//void Player::draw(sf::RenderWindow& window, bool alignRight, std::shared_ptr<Monster> selectedFromHand) const
//{
//	drawHand(window, alignRight, selectedFromHand);
//}

//void Player::drawHealth

void Player::drawKeys(sf::RenderWindow& window, bool alignRight) const
{
    const sf::Font& font = AssetsManager::getInstance().getFont("Lilita");

    sf::Text keysText(font);
    //keysText.setString("Keys: " + std::to_string(m_keys));
    keysText.setString("Keys: " + std::to_string(m_keys) + " / " + std::to_string(m_maxKeys));
    keysText.setCharacterSize(24);
    keysText.setFillColor(sf::Color::White);

    // שומרים את ה-bounds של הטקסט
    auto bounds = keysText.getLocalBounds();

    // --- חישוב מיקום בציר X (ימין או שמאל) ---
    // SFML 3: משתמשים ב-bounds.size.x במקום ב-width
    float xPos = alignRight ? Config::WINDOW_WIDTH - 20.f - bounds.size.x : 20.f;

    // --- חישוב מיקום בציר Y (מרכז אנכי מושלם) ---
    // SFML 3: משתמשים ב-bounds.size.y וב-bounds.position.y
    float yPos = 20.f - bounds.position.y;

    keysText.setPosition({ xPos, yPos });

    window.draw(keysText);
}
//
//void Player::drawHand(sf::RenderWindow& window, bool alignRight, Monster* selectedFromHand) const
//{
//    // 1. ציור "השולחן" (הפאנל התחתון)
//    sf::RectangleShape bottomPanel({ static_cast<float>(Config::WINDOW_WIDTH), Config::BOTTOM_PANEL_HEIGHT });
//    bottomPanel.setPosition({ 0.f, Config::BOTTOM_PANEL_Y });
//    bottomPanel.setFillColor(sf::Color(40, 40, 40));
//    window.draw(bottomPanel);
//
//    const sf::Font& font = AssetsManager::getInstance().getFont("Lilita");
//
//    for (size_t i = 0; i < m_monsters.size(); ++i)
//    {
//        // מפלצת שכבר זומנה ללוח (getQ != -1) לא תצויר ביד
//        //if (m_monsters[i]->isOnBoard()) continue;
//
//        float startX = alignRight ?
//            (Config::WINDOW_WIDTH - 20.f - Config::CARD_WIDTH - (i * Config::CARD_SPACING)) :
//            (20.f + (i * Config::CARD_SPACING));
//
//        bool isSelected = (selectedFromHand && m_monsters[i].get() == selectedFromHand);
//
//        //change this!!!! be more phol
//        m_monsters[i]->drawAsCard(window, { startX, Config::CARD_START_Y }, isSelected, m_monsters[i]->getCost() > m_keys);
//        // --- ציור מסגרת זהב אם הקלף נבחר ---
//        /*if (isSelected)
//        {
//            sf::RectangleShape selectionBorder({ Config::CARD_WIDTH + 6.f, Config::CARD_HEIGHT + 6.f });
//            selectionBorder.setPosition({ startX - 3.f, Config::CARD_START_Y - 3.f });
//            selectionBorder.setFillColor(sf::Color::Transparent);
//            selectionBorder.setOutlineThickness(3.f);
//            selectionBorder.setOutlineColor(sf::Color(255, 220, 50)); // Gold
//            window.draw(selectionBorder);
//        }
//
//        // --- ציור המפלצת (הקלף עצמו) ---
//        // הסרנו את ה-try-catch כדי שאם יש בעיית שמות אנחנו נראה אותה מיד ולא ננחש
//        std::string texKey = m_monsters[i]->getCardTextureKey();
//        const sf::Texture& monsterTex = AssetsManager::getInstance().getTexture(texKey);
//        sf::Sprite monsterSprite(monsterTex);
//
//        float scaleX = Config::CARD_WIDTH / static_cast<float>(monsterTex.getSize().x);
//        float scaleY = Config::CARD_HEIGHT / static_cast<float>(monsterTex.getSize().y);
//
//        float currentX = startX;
//        float currentY = Config::CARD_START_Y;
//
//        // הגדלה קלה אם הקלף נבחר
//        if (isSelected)
//        {
//            float enlarge = 1.08f;
//            scaleX *= enlarge;
//            scaleY *= enlarge;
//            float offsetX = (Config::CARD_WIDTH * (enlarge - 1.f)) / 2.f;
//            float offsetY = (Config::CARD_HEIGHT * (enlarge - 1.f)) / 2.f;
//            currentX -= offsetX;
//            currentY -= offsetY;
//        }
//
//        monsterSprite.setPosition({ currentX, currentY });
//        monsterSprite.setScale({ scaleX, scaleY });
//        window.draw(monsterSprite);
//
//        // --- ציור עלות המפתחות בתוך הקלף ---
//        sf::Text costText(font);
//        costText.setString(std::to_string(m_monsters[i]->getCost()));
//        costText.setCharacterSize(22);
//
//        // חיווי ויזואלי: אם הקלף יקר מדי, נצבע באדום, אחר כך בלבן
//        if (m_monsters[i]->getCost() > m_keys)
//            costText.setFillColor(sf::Color(255, 100, 100)); // אדום
//        else
//            costText.setFillColor(sf::Color::White);
//
//        costText.setOutlineColor(sf::Color::Black);
//        costText.setOutlineThickness(2.f);
//        costText.setPosition({ currentX + 10.f, currentY + 5.f }); // פינה שמאלית עליונה של הקלף
//
//        window.draw(costText);*/
//    }
//}
void Player::drawHand(sf::RenderWindow& window, bool alignRight, Card* selectedFromHand) const
{
    sf::RectangleShape bottomPanel({ static_cast<float>(Config::WINDOW_WIDTH), Config::BOTTOM_PANEL_HEIGHT });
    bottomPanel.setPosition({ 0.f, Config::BOTTOM_PANEL_Y });
    bottomPanel.setFillColor(sf::Color(40, 40, 40));
    window.draw(bottomPanel);

    for (size_t i = 0; i < m_hand.size(); ++i)
    {
        float startX = alignRight ?
            (Config::WINDOW_WIDTH - 20.f - Config::CARD_WIDTH - (i * Config::CARD_SPACING)) :
            (20.f + (i * Config::CARD_SPACING));

        bool isSelected = (selectedFromHand && m_hand[i].get() == selectedFromHand);

        // תיקון: <= בודק אם יש מספיק מפתחות
        bool enoughKeys = (m_hand[i]->getCost() <= m_keys);

        m_hand[i]->draw(window, { startX, Config::CARD_START_Y }, isSelected, enoughKeys);
    }
}

//std::shared_ptr<Monster> Player::handleHandClick(sf::Vector2f mousePos, bool alignRight) const
//Monster* Player::handleHandClick(sf::Vector2f mousePos, bool alignRight) const
//{
//    if (mousePos.y < Config::BOTTOM_PANEL_Y) return nullptr;
//
//    for (size_t i = 0; i < m_monsters.size(); ++i)
//    {
//        // אם המפלצת כבר על הלוח, אי אפשר לבחור אותה שוב מהיד
//        //if (m_monsters[i]->getQ() != -1) continue;
//        //if the monster is on the board- we cant choose her again from the hand
//        if (m_monsters[i]->isOnBoard()) continue;
//
//        float startX = alignRight ?
//            (Config::WINDOW_WIDTH - 20.f - Config::CARD_WIDTH - (i * Config::CARD_SPACING)) :
//            (20.f + (i * Config::CARD_SPACING));
//
//        // שינוי כאן: המפלצת בודקת בעצמה האם לחצו עליה כקלף במיקום הנוכחי
//        if (m_monsters[i]->isCardClicked(mousePos, { startX, Config::CARD_START_Y }))
//        {
//            // בדיקת המפתחות נשארת כאן (כי זה חוק של השחקן)
//            if (m_monsters[i]->getCost() > m_keys) return nullptr;
//
//            return m_monsters[i].get();
//        }
//   //     float startX;
//   //     if (!alignRight)
//   //         startX = 20.f + (i * Config::CARD_SPACING);
//   //     else
//   //         startX = Config::WINDOW_WIDTH - 20.f - Config::CARD_WIDTH - (i * Config::CARD_SPACING); // תוקן ה-1280!
//
//   //     // יצירת "מלבן וירטואלי" סביב הקלף עם המידות מהקבועים
//   //     sf::FloatRect cardRect({ startX, Config::CARD_START_Y }, { Config::CARD_WIDTH, Config::CARD_HEIGHT });
//   //     if (cardRect.contains(mousePos))
//   //     {
//			//if (m_monsters[i]->getCost() > m_keys) return nullptr; // אם אין מספיק מפתחות, לא ניתן לבחור את הקלף
//   //         return m_monsters[i]; // מצאנו איזה קלף נלחץ!
//   //     }
//    }
//    return nullptr;
//}
Card* Player::handleHandClick(sf::Vector2f mousePos, bool alignRight) const
{
    if (mousePos.y < Config::BOTTOM_PANEL_Y) return nullptr;

    for (size_t i = 0; i < m_hand.size(); ++i)
    {
        float startX = alignRight ?
            (Config::WINDOW_WIDTH - 20.f - Config::CARD_WIDTH - (i * Config::CARD_SPACING)) :
            (20.f + (i * Config::CARD_SPACING));

        if (m_hand[i]->isCardClicked(mousePos, { startX, Config::CARD_START_Y }))
        {
            // תיקון: התעלמות מקלפים שכבר על הלוח או שאין מספיק מפתחות עבורם
            if (/*m_hand[i]->isPlayed() ||*/ m_hand[i]->getCost() > m_keys)
                return nullptr;

            return m_hand[i].get();
        }
    }
    return nullptr;
}

Monster* Player::playCard(Card* card)
{
    if (!card || card->isPlayed() || card->getCost() > m_keys)
        return nullptr;

    std::unique_ptr<Monster> monster = card->spawnMonster();

    // הגנה: אם יצירת המפלצת נכשלה, לא מוסיפים nullptr לווקטור
    if (!monster) return nullptr;

    reduceKeys(card->getCost());

    Monster* raw = monster.get();
    m_monsters.push_back(std::move(monster));

    return raw;
}

void Player::removeDeadMonsters()
{
    // שלב 1: ניתוק קלפים ממונעי dangling pointers
    for (auto& card : m_hand)
    {
        if (card && card->getLinkedMonster() && !card->getLinkedMonster()->isAlive())
            card->clearLink();
    }

    // שלב 2: ניתוק מפלצות מתות מהאריחים
    for (auto& monster : m_monsters)
    {
        if (monster && !monster->isAlive() && monster->getCurrentTile())
            monster->getCurrentTile()->clearEntity();
    }

    // שלב 3: מחיקת מצביעי null ומפלצות מתות
    m_monsters.erase(
        std::remove_if(m_monsters.begin(), m_monsters.end(),
            [](const std::unique_ptr<Monster>& m) {
                return !m || !m->isAlive();
            }),
        m_monsters.end());
}

void Player::endTurn()
{
    // בדיקת תקינות המצביע לפני קריאה ל-isOnBoard()
    for (auto& monster : m_monsters)
    {
        if (monster && monster->isOnBoard())
            monster->resetActions();
    }

    if (m_maxKeys == 0) return;

    m_keys += 3;
    m_maxKeys = std::max(0, m_maxKeys - 3);
}
//Monster* Player::playCard(Card* card)
//{
//    if (!card || card->isPlayed() || card->getCost() > m_keys)
//        return nullptr;
//
//    reduceKeys(card->getCost());
//
//    std::unique_ptr<Monster> monster = card->spawnMonster();
//    Monster* raw = monster.get();
//    m_monsters.push_back(std::move(monster));
//
//    return raw;
//}
//
//void Player::removeDeadMonsters()
//{
//    // שלב 1: מנתקים Card->m_linkedMonster לפני שהמפלצת נהרסת (מונע dangling pointer)
//    for (auto& card : m_hand)
//    {
//        if (card->getLinkedMonster() != nullptr && !card->getLinkedMonster()->isAlive())
//            card->clearLink();
//    }
//
//    // שלב 2: מנתקים מה-Tile
//    for (auto& monster : m_monsters)
//    {
//        if (!monster->isAlive() && monster->getCurrentTile() != nullptr)
//            monster->getCurrentTile()->clearEntity();
//    }
//
//    // שלב 3: מוחקים בפועל
//    m_monsters.erase(
//        std::remove_if(m_monsters.begin(), m_monsters.end(),
//            [](const std::unique_ptr<Monster>& m) { return !m->isAlive(); }),
//        m_monsters.end());
//}
//
//
//void Player::endTurn()
//{
//    for (auto& monster : m_monsters)
//        if (monster->isOnBoard())
//            monster->resetActions();
//
//	if (m_maxKeys == 0) return;
//    // Add 3 keys
//    m_keys += 3;
//
//    // Decrease max capacity by 3, but don't go below 0
//    m_maxKeys = std::max(0, m_maxKeys - 3);
//    // Cap current keys to the new max
//    /*if (m_keys > m_maxKeys)
//        m_keys = m_maxKeys;*/
//    
//}

void Player::reduceKeys(int cost)
{
	m_keys -= cost;
}

Heart* Player::getHeart() 
{
    return m_heart.get();
}

bool Player::isDead() const
{
    return !(m_heart->isAlive());
}
