#include "Player.h"
#include "Monsters/MonsterFactory.h"
#include "Constants.h"
#include "Tiles/Tile.h"
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
void Player::draw(sf::RenderWindow& window, bool alignRight, const Card* selectedFromHand) const
{
    drawHand(window, alignRight, selectedFromHand);
	drawKeys(window, alignRight);
	//למה הוא לא קורא לdrawKeys? כי הוא לא קורא לdrawKeys, צריך להוסיף את זה פה
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
    float xPos = alignRight ? static_cast<float>(Config::WINDOW_WIDTH) - 20.f - bounds.size.x : 20.f;

    // --- חישוב מיקום בציר Y (מרכז אנכי מושלם) ---
    // SFML 3: משתמשים ב-bounds.size.y וב-bounds.position.y
    float yPos = 20.f - bounds.position.y;

    keysText.setPosition({ xPos, yPos });

    window.draw(keysText);
}

void Player::drawHand(sf::RenderWindow& window, bool alignRight, const Card* selectedFromHand) const
{
    //למה זה בשחקן?
    /*sf::RectangleShape bottomPanel({ static_cast<float>(Config::WINDOW_WIDTH), Config::BOTTOM_PANEL_HEIGHT });
    bottomPanel.setPosition({ 0.f, Config::BOTTOM_PANEL_Y });
    bottomPanel.setFillColor(sf::Color(40, 40, 40));
    window.draw(bottomPanel);*/

    for (size_t i = 0; i < m_hand.size(); ++i)
    {
        // Dead code, removed (kept as a comment, not deleted): startX was
        // computed here but never actually read - getCardPosition(i, ...)
        // below already redoes the exact same computation itself.
        // float startX = alignRight ?
        //     (Config::WINDOW_WIDTH - 20.f - Card::WIDTH - (i * CARD_SPACING)) :
        //     (20.f + (i * CARD_SPACING));

        bool isSelected = (selectedFromHand && m_hand[i].get() == selectedFromHand);

        // תיקון: <= בודק אם יש מספיק מפתחות
        bool enoughKeys = (m_hand[i]->getCost() <= m_keys);

        m_hand[i]->draw(window, getCardPosition(i, alignRight), isSelected, enoughKeys);
    }
}

// Player.cpp
std::string Player::getCardTooltipAt(const sf::Vector2f& pos) const
{
    Card* card = getCardAtPosition(pos, m_side == PlayerSide::Right);
    if (!card)
        return "";

    // 3. הגנה: בדיקה אם לקלף יש מפלצת מקושרת (m_linkedMonster != nullptr)
    const Monster* monster = card->getLinkedMonster(); // רק קריאה - getSpecialAbilityDescription() למטה הוא const
    // אותו שורש כמו canUseSpecialAbilityNow: הקלף נשאר מקושר גם אחרי מוות
    // (removeDeadMonsters אף פעם לא נקראת) - בלי isAlive() כאן, הטולטיפ
    // ממשיך להופיע על קלף של מפלצת מתה.
    if (!monster || !monster->isAlive())
        return "";

    // 4. החזרת התיאור בבטחה
    return monster->getSpecialAbilityDescription();
    //Card* card = getCardAtPosition(pos, m_side == PlayerSide::Right); // Determine alignment based on player side
    //Monster* monster = card->getLinkedMonster();
    //if (monster)
    //    return monster->getSpecialAbilityDescription();
    ///*for (const auto& card : m_hand)
    //{
    //    if (card->getBounds().contains(pos))
    //    {
    //        Monster* monster = card->getLinkedMonster();
    //        if (monster)
    //            return monster->getSpecialAbilityDescription();
    //    }
    //}*/
    //return ""; // אם העכבר לא מעל אף קלף
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
    if (mousePos.y < BOTTOM_PANEL_TOP_Y) return nullptr;

    Card* card = getCardAtPosition(mousePos, alignRight);
    if (card && !card->isPlayed() && card->getCost() > m_keys)
        return nullptr;

    return card;
    //if (mousePos.y < Config::BOTTOM_PANEL_Y) return nullptr;

    //for (size_t i = 0; i < m_hand.size(); ++i)
    //{
    //    /*float startX = alignRight ?
    //        (Config::WINDOW_WIDTH - 20.f - Config::CARD_WIDTH - (i * Config::CARD_SPACING)) :
    //        (20.f + (i * Config::CARD_SPACING));*/

    //    if (m_hand[i]->isCardClicked(mousePos, getCardPosition(i,alignRight) ))
    //    {
    //        if (!m_hand[i]->isPlayed() &&
    //            m_hand[i]->getCost() > m_keys)
    //        {
    //            return nullptr;
    //        }

    //        return m_hand[i].get();
    //    }
    //}
    //return nullptr;
}

Monster* Player::playCard(Card* card)
{
    if (!card || card->isPlayed() || card->getCost() > m_keys)
        return nullptr;

	//crate the monster and get a unique_ptr to it
    std::unique_ptr<Monster> monster = card->spawnMonster();

	//if the monster is nullptr, don't proceed
    if (!monster) return nullptr;

    reduceKeys(card->getCost());

    Monster* raw = monster.get();
    m_monsters.push_back(std::move(monster));

    return raw;
}

//למה אנחנו צריכים את הפונקציה הזאת? אם נעשה בהמשך משהו שמוחק מפלצת בקלף שהיא מתה. לא עדיף שזה ימחק מזה? יעילות
//האם צריך את הפונקציה הזאת בשביל להקטין את הקלפים שמישהו מת? שלא ישאר מקום ריק?????
void Player::removeDeadMonsters()
{
    // Old two-step version (unlink now, erase later), kept as a comment -
    // Card::isGone() is already true the instant its monster dies, so
    // erasing the whole Card directly (no separate unlink step first) is
    // safe here: GameplayState::update() clears its own m_selectedFromHand/
    // m_pendingSpecialCard first (the only other things that could ever
    // reference one of these Cards), specifically so this erase never runs
    // while something else still points at the Card being destroyed.
    //
    // for (auto& card : m_hand)
    // {
    //     if (card && card->getLinkedMonster() && card->getLinkedMonster()->isReadyForRemoval())
    //         card->clearLink();
    // }
    std::erase_if(m_hand, [](const std::unique_ptr<Card>& card) {
        return !card || card->isGone();
    });

	//unlink dead monsters from their tiles
    /*for (auto& monster : m_monsters)
    {
        if (monster && !monster->isAlive() && monster->getCurrentTile())
            monster->getCurrentTile()->clearEntity();
    }*/

	//delete the dead monsters from the vector
    /*m_monsters.erase(
        std::remove_if(m_monsters.begin(), m_monsters.end(),
            [](const std::unique_ptr<Monster>& m) {
                return !m || !m->isAlive();
            }),
        m_monsters.end());*/
    //std::erase_if(m_monsters, [](const auto& m) { return !m || !m->isAlive(); });
}

int Player::indexOfCard(const Card* card) const
{
    for (size_t i = 0; i < m_hand.size(); ++i)
    {
        if (m_hand[i].get() == card)
            return static_cast<int>(i);
    }
    return -1;
}

Card* Player::getCardAtPosition(const sf::Vector2f& mousePos, bool alignRight) const
{
    for (size_t i = 0; i < m_hand.size(); ++i)
    {
        if (m_hand[i] && m_hand[i]->isCardClicked(mousePos, getCardPosition(i, alignRight)))
            return m_hand[i].get();
    }
    return nullptr;
}

//לבדוק שהלוח לא עושה את הפעולות האלה. 
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
