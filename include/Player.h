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
    void draw(sf::RenderWindow& window, bool alignRight, std::shared_ptr<Monster> selectedFromHand = nullptr) const;
    std::shared_ptr<Monster> handleHandClick(sf::Vector2f mousePos, bool alignRight) const;
    //void draw(sf::RenderWindow& window) const;
    //const sf::Vector2f& getPosition() const;
	void endTurn();
    PlayerSide getSide() const { return m_side; }
    void reduceKeys(int cost);

private:
    void drawKeys(sf::RenderWindow& window, bool alignRight) const;
    void drawHand(sf::RenderWindow& window, bool alignRight, std::shared_ptr<Monster> selectedFromHand = nullptr) const;
	//std::unique_ptr<Heart> m_heart;
    // 
	//std::vector<std::unique_ptr<Card>> m_cards;
    int m_keys;
    int m_maxKeys;
    PlayerSide m_side;
    std::vector<std::shared_ptr<Monster>> m_monsters;
};
