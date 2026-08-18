#pragma once
#include <SFML/Graphics.hpp>
#include "Constants.h"
class Monster; // Forward declaration

enum class EntityType {
    Monster,
    Heart
};

class Tile; // Forward declaration

class BoardEntity {
public:
    BoardEntity(int q, int row, const sf::Vector2f& position, int health)
        : m_q(q), m_row(row), m_screenPos(position), m_health(health), m_maxHealth(health)/*, m_currentTile(nullptr)*/ {
    }

    // שינוי קריטי: הדיסטרקטור עכשיו וירטואלי אבל לא default, נממש אותו ב-cpp
    virtual ~BoardEntity() = default;

    //virtual int getRange() const { return 0; }
    // התאמה מדוייקת לחתימה שלכם:
    virtual void draw(sf::RenderWindow& window, PlayerSide currentTurnSide) const = 0;
    virtual void takeDamage(int damage);
    virtual bool isAlive() const;
    //virtual int getHealth() const = 0;
    virtual PlayerSide getSide() const = 0;
    bool isEnemyOf(PlayerSide otherSide) const { return getSide() != otherSide; }
    bool isAllyOf(PlayerSide otherSide) const { return getSide() == otherSide; }
    virtual void attack(BoardEntity* target) {}

    virtual void setSelected(bool selected) { m_isSelected = selected; }
    virtual bool isSelected() const { return m_isSelected; }
    virtual int getRange() const { return 0; }
    //virtual EntityType getType() const = 0;
    // 
    //virtual bool isSelectable() const { return false; } // ברירת מחדל: לב אי אפשר לבחור
    virtual bool canBeSelectedBy(PlayerSide side) const { return false; }

    //virtual bool isSelected() const { return false; }
    virtual bool isMoving() const { return false; }
    //// --- פונקציות חדשות: קשר דו-כיווני עם המשבצת ---
    //void setCurrentTile(Tile* tile) { m_currentTile = tile; }
    //Tile* getCurrentTile() const { return m_currentTile; }
    void setCurrentTile(Tile* tile) { m_currentTile = tile; }
    Tile* getCurrentTile() const { return m_currentTile; }


    // --- פונקציות עזר למיקום (יעזור מאוד ללוח ולמפלצת עצמה) ---
    int getQ() const { return m_q; }
    int getRow() const { return m_row; }
    void setCoords(int q, int row) { m_q = q; m_row = row; }

    virtual void spawnOnBoard(int q, int row, const sf::Vector2f& screenPos);
    virtual void update(float dt) {}
    virtual bool canFly() const { return false; }
    /*sf::Vector2f getScreenPosition() const { return m_screenPos; }
    void setScreenPosition(const sf::Vector2f& pos) { m_screenPos = pos; }*/
    virtual Monster* asMonster() { return nullptr; }

protected:
    void drawHealthBar(sf::RenderWindow& window) const;
    // המשתנים מוגנים כדי שגם מפלצת וגם לב יוכלו לגשת אליהם ישירות במידת הצורך
    int m_q;
    int m_row;
    sf::Vector2f m_screenPos;
    int m_health;
    int m_maxHealth;
    Tile* m_currentTile;
    bool m_isSelected = false;
};