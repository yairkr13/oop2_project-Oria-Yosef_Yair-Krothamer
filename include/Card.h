#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <memory>
#include "Constants.h"

class Monster;

// One hand card: spawns a Monster onto the board when played, and stays
// linked to it afterward so its cooldown/status can be shown on the card.
class Card
{
public:
    // A card's own on-screen size. Public so Player can reference it when spacing a row of cards.
    static constexpr float WIDTH = 80.f;
    static constexpr float HEIGHT = 100.f;

    Card(std::string monsterId, int cost, std::string textureKey, PlayerSide side);

    void draw(sf::RenderWindow& window, sf::Vector2f position, bool isSelected, bool enoughKeys) const;
    bool isCardClicked(sf::Vector2f mousePos, sf::Vector2f cardPosition) const;

    int getCost() const;
    PlayerSide getSide() const;

    // True once this card has a live monster linked to it on the board.
    bool isPlayed() const;

    // Whether this card should be treated as gone entirely (not drawn, not
    // clickable). True the instant its linked monster dies - Player::removeDeadMonsters()
    // erases the Card outright as soon as this becomes true.
    // (Defined out-of-line: Monster is only forward-declared here.)
    bool isGone() const;

    // Read-only access, for callers that only ask the linked monster something.
    const Monster* getLinkedMonster() const;

    // Mutable access - only for callers that change the monster itself (useSpecialAbility, cancelSpecialAbility).
    Monster* getMutableLinkedMonster() const;

    // Creates a new Monster, transfers ownership out, keeps an observer pointer to it.
    std::unique_ptr<Monster> spawnMonster();

    // No longer called anywhere (kept, not deleted): Player::removeDeadMonsters()
    // now erases the whole Card via isGone() directly instead of unlinking first.
    // void clearLink() { m_linkedMonster = nullptr; }

private:
    void drawBoarder(sf::RenderWindow& window, sf::Vector2f position, sf::Color color) const;
    void drawCostText(sf::RenderWindow& window, sf::Vector2f drawPos, const sf::Font& font, bool enoughKeys) const;
    void drawStatusText(sf::RenderWindow& window, sf::Vector2f drawPos, const sf::Font& font) const;

    std::string getCardTextureKey() const;

    std::string m_monsterId;
    int m_cost;
    std::string m_textureKey;
    PlayerSide m_side;
    Monster* m_linkedMonster = nullptr; // only observer, Player is the owner
};
