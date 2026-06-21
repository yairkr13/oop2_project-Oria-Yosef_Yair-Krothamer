#pragma once
#include "GameObject.h"
#include <string>

class Monster //: public GameObject
{
public:
    Monster(const std::string& name, int health, int attackPower, int range, int cost, int q, int row, sf::Color color, const std::string& textureKey);
    virtual ~Monster() = default;

    virtual void attack(Monster& target) = 0;

    void draw(sf::RenderWindow& window) const;
    void takeDamage(int damage);
    bool isAlive() const;
    bool contains(sf::Vector2f point, sf::Vector2f screenPos) const;

    void setSelected(bool selected) { m_selected = selected; }
    bool isSelected() const { return m_selected; }

    int getQ() const { return m_q; }
    int getRow() const { return m_row; }
    void setPosition(int q, int row) { m_q = q; m_row = row; }
    int getRange() const { return m_range; }
    const std::string& getName() const { return m_name; }
    void setScreenPosition(const sf::Vector2f& pos) { m_screenPos = pos; }

protected:
    std::string m_name;
    int m_health;
    int m_attackDamage;
    int m_range;
    int m_cost;
    int m_q;
    int m_row;
    bool m_selected = false;
    sf::Color m_color;

    std::string m_textureKey;
    sf::Vector2f m_screenPos;
	//MovementType m_movementType;
	//belong to faction????????????????????????????
};