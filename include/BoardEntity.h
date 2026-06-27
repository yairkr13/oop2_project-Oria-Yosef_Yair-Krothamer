#pragma once
#include <SFML/Graphics.hpp>
#include "Constants.h"

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

    // התאמה מדוייקת לחתימה שלכם:
    virtual void draw(sf::RenderWindow& window, PlayerSide currentTurnSide) const = 0;
    virtual void takeDamage(int damage);
    virtual bool isAlive() const;
    //virtual int getHealth() const = 0;
    virtual PlayerSide getSide() const = 0;
    virtual EntityType getType() const = 0;
    // 
    //// --- פונקציות חדשות: קשר דו-כיווני עם המשבצת ---
    //void setCurrentTile(Tile* tile) { m_currentTile = tile; }
    //Tile* getCurrentTile() const { return m_currentTile; }

    // --- פונקציות עזר למיקום (יעזור מאוד ללוח ולמפלצת עצמה) ---
    int getQ() const { return m_q; }
    int getRow() const { return m_row; }
    void setCoords(int q, int row) { m_q = q; m_row = row; }

    virtual void spawnOnBoard(int q, int row, const sf::Vector2f& screenPos);
    /*sf::Vector2f getScreenPosition() const { return m_screenPos; }
    void setScreenPosition(const sf::Vector2f& pos) { m_screenPos = pos; }*/
protected:
    void drawHealthBar(sf::RenderWindow& window) const;
    // המשתנים מוגנים כדי שגם מפלצת וגם לב יוכלו לגשת אליהם ישירות במידת הצורך
    int m_q;
    int m_row;
    sf::Vector2f m_screenPos;
    int m_health;
    int m_maxHealth;
    //Tile* m_currentTile;
};