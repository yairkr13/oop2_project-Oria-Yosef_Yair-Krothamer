#include "Player.h"
#include "Monsters/MonsterFactory.h"
#include "Constants.h"
#include "Tiles/Tile.h"
#include "AssetsManager.h"

Player::Player(PlayerSide side)
	: m_heart(std::make_unique<Heart>(side, -1, -1, sf::Vector2f(0.f, 0.f))), // יוצרים את הלב עם מיקום זמני
    m_keys(3),m_maxKeys(12), m_side(side)
{
    m_hand = MonsterFactory::createStandardHand(side);
}

void Player::draw(sf::RenderWindow& window, bool alignRight, const Card* selectedFromHand) const
{
    drawHand(window, alignRight, selectedFromHand);
	drawKeys(window, alignRight);
	//למה הוא לא קורא לdrawKeys? כי הוא לא קורא לdrawKeys, צריך להוסיף את זה פה
}

void Player::drawKeys(sf::RenderWindow& window, bool alignRight) const
{
    const sf::Font& font = AssetsManager::getInstance().getFont("Lilita");

    sf::Text keysText(font);
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
    for (size_t i = 0; i < m_hand.size(); ++i)
    {
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
}

Card* Player::handleHandClick(sf::Vector2f mousePos, bool alignRight) const
{
    if (mousePos.y < BOTTOM_PANEL_TOP_Y) return nullptr;

    Card* card = getCardAtPosition(mousePos, alignRight);
    // A clicked card that's already played is still returned (so it can be
    // selected for a Special), but a not-yet-played card the player can't
    // afford is treated as if nothing was clicked at all.
    if (card && !card->isPlayed() && card->getCost() > m_keys)
        return nullptr;

    return card;
}

Monster* Player::playCard(Card* card)
{
    // Reject a missing card, one already played, or one this player can't
    // currently afford.
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
    // Card::isGone() is already true the instant its monster dies, so
    // erasing the whole Card directly (no separate unlink step first) is
    // safe here: GameplayState::update() clears its own m_selectedFromHand/
    // m_pendingSpecialCard first (the only other things that could ever
    // reference one of these Cards), specifically so this erase never runs
    // while something else still points at the Card being destroyed.
    std::erase_if(m_hand, [](const std::unique_ptr<Card>& card) {
        return !card || card->isGone();
    });
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
        // Skip a null slot defensively, and only match a card whose own
        // on-screen rect actually contains this click.
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

    // Once the per-game key cap has been fully handed out, stop granting
    // more each turn - m_keys can still be spent down and never regrows past this.
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
