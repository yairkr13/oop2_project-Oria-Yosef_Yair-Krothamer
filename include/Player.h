#pragma once
#include <SFML/Graphics.hpp>
#include "StaticObject.h"
#include "Heart.h"
#include "Card.h"
#include "Constants.h"
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
    void draw(sf::RenderWindow& window, bool alignRight, Monster* selectedFromHand = nullptr) const;
    void drawKeys(sf::RenderWindow& window, bool alignRight) const;

    // �����: �������� ������ ����� ���� (�� �� ����� �� "����" �� ����, ��� �� ���� �����)
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

    // Behavioral turn hooks (see AIPlayer for the self-driving override).
    // Base Player is purely reactive: it never acts on its own, so these are
    // no-ops and it's never busy - it just waits for external input (mouse
    // clicks / Space) to end its turn.
    virtual void onTurnStart(Board& board) {}
    virtual void updateTurn(Board& board) {}
    virtual bool isBusy() const { return false; }

//private:
protected:
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
    //std::vector<Card> m_cards;
};
