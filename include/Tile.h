#pragma once
#include "SFML/Graphics.hpp"
#include "Monsters/Monster.h"
#include "BoardEntity.h"
#include <memory>
//#include "StaticObject.h"

class Tile //: public StaticObject
{
public:
	Tile(int q, int row, const sf::Vector2f& position, const sf::Color& color= sf::Color(80, 80, 80, 180));
    virtual ~Tile() = default; // 1. חובה עבור פולימורפיזם וירושה בטוחה!
	void draw(sf::RenderWindow& window, PlayerSide currentTurnSide) const;
	int getQ() const { return m_q; }
	int getRow() const { return m_row; }
	bool isPassable() const { return m_isPassable; }
	bool isHighlighted() const { return m_isHighlighted; }
	virtual void setHighlighted(bool highlighted, const sf::Color& highlightColor= sf::Color(150, 220, 150, 180));

	//void setMonster(std::shared_ptr<Monster> monster) { m_monsterRef = monster; };
	//std::shared_ptr<Monster> getMonster() const { return m_monsterRef.lock(); }
	//bool hasMonster() const { return !m_monsterRef.expired(); }
    // --- ניהול הישות ---
    void clearEntity();

    BoardEntity* getEntity() const { return m_entity; }

    // פונקציה ספציפית - מיועדת למערכת הקרב (שצריכה תכלס מפלצת)
    //Monster* getMonster() const {
    //    if (m_entity && m_entity->getType() == EntityType::Monster) {
    //        return static_cast<Monster*>(m_entity); // Downcasting בטוח
    //    }
    //    return nullptr;
    //}

    void setEntity(BoardEntity* entity);
    bool hasEntity() const { return m_entity != nullptr; }

    bool isOccupiedByEnemy(PlayerSide mySide) const { return m_entity != nullptr && m_entity->isEnemyOf(mySide); }
    bool isOccupiedByAlly(PlayerSide mySide) const { return m_entity != nullptr && m_entity->isAllyOf(mySide); }

    // עוד עטיפה נוחה - "יש כאן ישות מתה שצריך לפנות?" (משמש ב-updateTileEffects)
    bool hasDeadEntity() const { return m_entity != nullptr && !m_entity->isAlive(); }

    virtual bool isPassableFor(Monster* monster) const {
        return m_isPassable;
    }
    //void setMonster(Monster* monster) { setEntity(monster); }

    /*bool hasMonster() const {
        return m_entity != nullptr && m_entity->getType() == EntityType::Monster;
    }*/
    virtual bool isHole() const { return false; }

    virtual void applyTileEffect() {}
protected:
    sf::CircleShape m_shape;
    bool m_isPassable;
    sf::Color m_color;
private:
	int m_row;
	int m_q;
	bool m_isHighlighted = false;
	//std::weak_ptr<BoardEntity> m_entityRef;
	BoardEntity* m_entity = nullptr;
};