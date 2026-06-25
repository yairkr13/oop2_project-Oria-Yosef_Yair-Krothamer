#pragma once
#include "GameObject.h"
#include "Constants.h"
#include <string>

class Monster
{
public:
    Monster(const std::string& name, int health, int attackPower, int range, int cost, int q, int row, sf::Color color, const std::string& textureKey);
    virtual ~Monster() = default;

    void draw(sf::RenderWindow& window) const;
    void takeDamage(int damage);
    bool isAlive() const;
    bool contains(sf::Vector2f point, sf::Vector2f screenPos) const;
	void attack(std::shared_ptr<Monster> target) { target->takeDamage(m_attackDamage); }

    void setSelected(bool selected) { m_selected = selected; }
    bool isSelected() const { return m_selected; }

    int getQ() const { return m_q; }
    int getRow() const { return m_row; }
    void setPosition(int q, int row) { m_q = q; m_row = row; }
	int getCost() const { return m_cost; }
    int getRange() const { return m_range; }
    const std::string& getName() const { return m_name; }
    //std::string getTextureKey() const { return m_textureKey + (m_side == PlayerSide::Left ? "_r" : "_l"); }
    //std::string getCardTextureKey() const { return m_textureKey + "_card_" + (m_side == PlayerSide::Left ? "r" : "l"); }
    void setScreenPosition(const sf::Vector2f& pos) { m_screenPos = pos; }

    void setSide(PlayerSide side) { m_side = side; }
    PlayerSide getSide() const { return m_side; }

    void walkTo(const sf::Vector2f& targetScreenPos);
    void update(float dt);
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
    PlayerSide m_side = PlayerSide::Left;

    std::string m_textureKey;
    sf::Vector2f m_screenPos;
    sf::Vector2f m_targetPos;
    bool m_isMoving = false;
    float m_speed = 300.f;

    bool m_hasTexture = true;
    float m_baseScale = 1.0f;
    mutable sf::Sprite m_sprite;
};
