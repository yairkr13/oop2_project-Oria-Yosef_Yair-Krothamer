#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <memory>
#include "Constants.h"

class Monster;

class Card
{
public:
    // Moved here from Config (Constants.h) - a card's own on-screen size is
    // Card's own business, not something every unrelated file including
    // Constants.h needs visibility into. Public so Player (which lays out a
    // row of cards, and needs to know how wide/tall one is to space them)
    // can reference Card::WIDTH/HEIGHT directly instead of a separate copy.
    static constexpr float WIDTH = 80.f;
    static constexpr float HEIGHT = 100.f;

    Card(std::string monsterId, int cost, std::string textureKey, PlayerSide side);

    void draw(sf::RenderWindow& window, sf::Vector2f position, bool isSelected, bool enoughKeys) const;
    bool isCardClicked(sf::Vector2f mousePos, sf::Vector2f cardPosition) const;

    int getCost() const;
    PlayerSide getSide() const;

    // האם הקלף כבר "שוחק" - יש לו מפלצת חיה מקושרת על הלוח
    bool isPlayed() const;

    // Whether this card should be treated as gone entirely - not drawn (see
    // draw()), not clickable (see Player::getCardAtPosition), and the
    // condition Player::removeDeadMonsters() erases this Card on. True the
    // instant this card's linked monster dies - no separate "remember it
    // died after unlinking" state needed, because removeDeadMonsters() now
    // erases the Card outright as soon as this becomes true, in the same
    // step, instead of unlinking now and erasing later.
    // (Defined out-of-line in Card.cpp: Monster is only forward-declared
    // here, and ->isAlive() needs the complete type.)
    bool isGone() const;

    // Safe default: read-only access, for callers that only ask the linked
    // monster something (getSpecialAbilityDescription, isReadyForRemoval,
    // specialAbilityNeedsTarget...).
    const Monster* getLinkedMonster() const;

    // Mutable access - only for the few callers that actually need to
    // change the monster itself (useSpecialAbility, cancelSpecialAbility).
    // Same reasoning/naming as Tile::getMutableEntity().
    Monster* getMutableLinkedMonster() const;

    // יוצר Monster חדש, מעביר בעלות החוצה, אבל שומר observer pointer לעצמו
    std::unique_ptr<Monster> spawnMonster();

    // Player קורא לזה כשהמפלצת מתה/יורדת מהלוח - מנתק את הקישור לפני שהיא נהרסת
    // No longer called anywhere (kept, not deleted): Player::removeDeadMonsters()
    // now erases the whole Card via isGone() directly instead of unlinking
    // first and erasing later - see there.
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
    Monster* m_linkedMonster = nullptr; //only observer, player is the owner
    // bool m_monsterDied = false; - removed, see isGone()/clearLink() above
};