#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <memory>
#include "Constants.h"

class Monster;

class Card
{
public:
    Card(std::string monsterId, int cost, std::string textureKey, PlayerSide side);

    void draw(sf::RenderWindow& window, sf::Vector2f position, bool isSelected, bool enoughKeys) const;
    bool isCardClicked(sf::Vector2f mousePos, sf::Vector2f cardPosition) const;

    int getCost() const { return m_cost; }
    PlayerSide getSide() const { return m_side; }

    // האם הקלף כבר "שוחק" - יש לו מפלצת חיה מקושרת על הלוח
    bool isPlayed() const { return m_linkedMonster != nullptr; }
    Monster* getLinkedMonster() const { return m_linkedMonster; }

    // יוצר Monster חדש, מעביר בעלות החוצה, אבל שומר observer pointer לעצמו
    std::unique_ptr<Monster> spawnMonster();

    // Player קורא לזה כשהמפלצת מתה/יורדת מהלוח - מנתק את הקישור לפני שהיא נהרסת
    void clearLink() { m_linkedMonster = nullptr; }

private:
    void drawBoarder(sf::RenderWindow& window, sf::Vector2f position, sf::Color color) const;
    void drawCostText(sf::RenderWindow& window, sf::Vector2f drawPos, const sf::Font& font, bool enoughKeys) const;
    void drawStatusText(sf::RenderWindow& window, sf::Vector2f drawPos, const sf::Font& font) const;

    std::string getCardTextureKey() const { return m_textureKey + "_card"; }

    std::string m_monsterId;
    int m_cost;
    std::string m_textureKey;
    PlayerSide m_side;
    Monster* m_linkedMonster = nullptr; // observer בלבד - Player הוא ה-owner
};