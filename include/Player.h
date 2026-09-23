#pragma once
#include <SFML/Graphics.hpp>
#include "Heart.h"
#include "Constants.h"
#include "Card.h"
#include <vector>
#include <memory>

class Board;

// One side's game state: its Heart, hand of Cards, on-board Monsters and
// keys. Purely reactive by default (see the turn hooks below) - AIPlayer
// and RemotePlayer override those hooks to drive themselves instead.
class Player
{
public:
    Player(PlayerSide side);
    virtual ~Player() = default;
    bool isDead() const;

    void draw(sf::RenderWindow& window, bool alignRight, const Card* selectedFromHand = nullptr) const;

    Card* handleHandClick(sf::Vector2f mousePos, bool alignRight) const;

    void endTurn();
    PlayerSide getSide() const;

    Heart* getHeart();

    // Behavioral turn hooks (see AIPlayer for the self-driving override).
    // Base Player is purely reactive: no-ops, never busy, just waits for
    // external input (mouse/Space) to end its turn.
    virtual void onTurnStart(Board& board);
    virtual void updateTurn(Board& board);
    virtual bool isBusy() const;

    Monster* playCard(Card* card);
    void removeDeadMonsters();
    std::string getCardTooltipAt(const sf::Vector2f& pos) const;

    // Turns a Card* (meaningless outside this process) into an index -
    // how GameplayState fills GameAction::cardIndex for RemotePlayer.
    // -1 if card isn't actually in this hand.
    int indexOfCard(const Card* card) const;
protected:
    int m_keys;

    std::vector<std::unique_ptr<Monster>> m_monsters;
    std::vector<std::unique_ptr<Card>> m_hand;
private:
    // AIPlayer (the only derived class) never touches these directly - it
    // goes through getSide()/getHeart() like everyone else.
    std::unique_ptr<Heart> m_heart;
    int m_maxKeys;
    PlayerSide m_side;

    static constexpr float CARD_SPACING = 110.f;
    static constexpr float CARD_TOP_MARGIN = 10.f; // below the bottom panel's own top edge

    // The bottom panel's top edge - shared with GameplayState, which draws the panel itself.
    static constexpr float BOTTOM_PANEL_TOP_Y = static_cast<float>(Config::WINDOW_HEIGHT) - Config::BOTTOM_PANEL_HEIGHT;

    Card* getCardAtPosition(const sf::Vector2f& mousePos, bool alignRight) const;

    sf::Vector2f getCardPosition(size_t index, bool alignRight) const;
    void drawKeys(sf::RenderWindow& window, bool alignRight) const;
    void drawHand(sf::RenderWindow& window, bool alignRight, const Card* selectedFromHand = nullptr) const;

    // Only ever called from within playCard() itself.
    void reduceKeys(int cost);
};
