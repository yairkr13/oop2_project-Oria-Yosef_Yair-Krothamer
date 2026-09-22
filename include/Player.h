#pragma once
#include <SFML/Graphics.hpp>
#include "Heart.h"
#include "Constants.h"
#include "Card.h" // <--- הכללה מלאה במקום class Card;
#include <vector>
#include <memory>

class Board;

//check
class Player //: public StaticObject do an heart tile
{
public:
    Player(PlayerSide side);
    virtual ~Player() = default; // <--- ������ �� ����� ���!
	//bool handleClick(const sf::Vector2f& pos);
    //Card* handleCardClick(sf::Vector2f mousePos);
    bool isDead() const;
    //align right - to delete and use side 

    // �����: ���� ������ ���� (���� ����) ���� ������ ������ ���
    //void draw(sf::RenderWindow& window, bool alignRight, Monster* selectedFromHand = nullptr) const;
    void draw(sf::RenderWindow& window, bool alignRight, Card* selectedFromHand = nullptr) const;

    Card* handleHandClick(sf::Vector2f mousePos, bool alignRight) const;

    /*void draw(sf::RenderWindow& window, bool alignRight, std::shared_ptr<Monster> selectedFromHand = nullptr) const;
    void drawKeys(sf::RenderWindow& window, bool alignRight) const;
    std::shared_ptr<Monster> handleHandClick(sf::Vector2f mousePos, bool alignRight) const;*/

    //void draw(sf::RenderWindow& window) const;
    //const sf::Vector2f& getPosition() const;
	void endTurn();
    PlayerSide getSide() const { return m_side; }
    void reduceKeys(int cost);

    Heart* getHeart();

    // Behavioral turn hooks (see AIPlayer for the self-driving override).
    // Base Player is purely reactive: it never acts on its own, so these are
    // no-ops and it's never busy - it just waits for external input (mouse
    // clicks / Space) to end its turn.
    virtual void onTurnStart(Board& board) {}
    virtual void updateTurn(Board& board) {}
    virtual bool isBusy() const { return false; }

    Monster* playCard(Card* card); // כבר לא מוחק מ-m_hand!
    void removeDeadMonsters();
    std::string getCardTooltipAt(const sf::Vector2f& pos) const;
//private:
protected:
    //void drawHand(sf::RenderWindow& window, bool alignRight, Monster* selectedFromHand = nullptr) const;
    //void drawHand(sf::RenderWindow& window, bool alignRight, Card* selectedFromHand = nullptr) const;
    //void drawKeys(sf::RenderWindow& window, bool alignRight) const;

	std::unique_ptr<Heart> m_heart;
    // 
	//std::vector<std::unique_ptr<Card>> m_cards;
    int m_keys;
    int m_maxKeys;
    PlayerSide m_side;

    //std::vector<std::shared_ptr<Monster>> m_monsters;
    std::vector<std::unique_ptr<Monster>> m_monsters;
    std::vector<std::unique_ptr<Card>> m_hand;
private:
    // Moved here from Config (Constants.h) - laying out a row of cards is
    // Player's own concern (how far apart it chooses to space them, how far
    // below the bottom panel's own top edge its own hand starts), not
    // something every unrelated file including Constants.h needs visibility
    // into. Card::WIDTH/HEIGHT (the card's own size, which Player also needs
    // here to right-align) stay on Card itself - see there.
    static constexpr float CARD_SPACING = 110.f;
    static constexpr float CARD_TOP_MARGIN = 10.f; // below the bottom panel's own top edge

    // The bottom panel's own top edge - genuinely shared with GameplayState
    // (which draws the panel itself). The window is a fixed size for its
    // whole lifetime (see Controller::Controller), so this is a plain
    // compile-time constant derived from Config, not something recomputed
    // per call.
    static constexpr float BOTTOM_PANEL_TOP_Y = static_cast<float>(Config::WINDOW_HEIGHT) - Config::BOTTOM_PANEL_HEIGHT;

    Card* getCardAtPosition(const sf::Vector2f& mousePos, bool alignRight) const;

    sf::Vector2f getCardPosition(size_t index, bool alignRight) const
    {
        float xPos = alignRight ?
            (static_cast<float>(Config::WINDOW_WIDTH) - 20.f - Card::WIDTH - (index * CARD_SPACING)) :
            (20.f + (index * CARD_SPACING));

        return { xPos, BOTTOM_PANEL_TOP_Y + CARD_TOP_MARGIN };
    }
    void drawKeys(sf::RenderWindow& window, bool alignRight) const;
    void drawHand(sf::RenderWindow& window, bool alignRight, Card* selectedFromHand = nullptr) const;
};
