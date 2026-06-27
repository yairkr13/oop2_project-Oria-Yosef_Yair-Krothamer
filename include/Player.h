#pragma once
#include <SFML/Graphics.hpp>
#include "StaticObject.h"
#include "Heart.h"
#include "Card.h"
#include "Constants.h"
#include <vector>   
#include <memory>
//check
class Player //: public StaticObject do an heart tile
{
public:
    Player(PlayerSide side);
	//bool handleClick(const sf::Vector2f& pos);
    Card* handleCardClick(sf::Vector2f mousePos);
    //align right - to delete and use side 

    // שינוי: מעבר למצביע רגיל (צופה בלבד) עבור המפלצת הנבחרת ביד
    void draw(sf::RenderWindow& window, bool alignRight, Monster* selectedFromHand = nullptr) const;
    void drawKeys(sf::RenderWindow& window, bool alignRight) const;

    // שינוי: הפונקציה מחזירה מצביע רגיל (כי מי שלוחץ רק "שואל" מי נבחר, הוא לא מקבל בעלות)
    Monster* handleHandClick(sf::Vector2f mousePos, bool alignRight) const;

    /*void draw(sf::RenderWindow& window, bool alignRight, std::shared_ptr<Monster> selectedFromHand = nullptr) const;
    void drawKeys(sf::RenderWindow& window, bool alignRight) const;
    std::shared_ptr<Monster> handleHandClick(sf::Vector2f mousePos, bool alignRight) const;*/

    //void draw(sf::RenderWindow& window) const;
    //const sf::Vector2f& getPosition() const;
	void endTurn();
    PlayerSide getSide() const { return m_side; }
    void reduceKeys(int cost);

    Heart* getHeart();

private:
    //void drawHand(sf::RenderWindow& window, bool alignRight, std::shared_ptr<Monster> selectedFromHand = nullptr) const;
    void drawHand(sf::RenderWindow& window, bool alignRight, Monster* selectedFromHand = nullptr) const;
	std::unique_ptr<Heart> m_heart;
    // 
	//std::vector<std::unique_ptr<Card>> m_cards;
    int m_keys;
    int m_maxKeys;
    PlayerSide m_side;
    //std::vector<std::shared_ptr<Monster>> m_monsters;
    std::vector<std::unique_ptr<Monster>> m_monsters;
};
