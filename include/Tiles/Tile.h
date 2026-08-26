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
    virtual ~Tile() = default; // 1. ���� ���� ����������� ������ �����!
	void draw(sf::RenderWindow& window, PlayerSide currentTurnSide) const;
	int getQ() const { return m_q; }
	int getRow() const { return m_row; }

	// This tile's own screen-space center, so callers that already hold a
	// Tile* (Board, mainly) can ask this instead of re-deriving the same
	// point from Board::tileToScreen(getQ(), getRow()). Mirrors
	// BoardEntity::getScreenPosition()'s existing shape.
	//
	// m_shape's own position is NOT the center - m_shape's origin is never
	// set (see Tile::Tile), so SFML anchors it at the hexagon's top-left
	// bounding-box corner, exactly Config::TILE_RADIUS short of the true
	// center in both axes. tileToScreen() already applies this same
	// correction when computing a center from raw coordinates; this must
	// apply it too, or the two stop agreeing on what "a tile's screen
	// position" means.
	sf::Vector2f getScreenPosition() const
	{
		return m_shape.getPosition() + sf::Vector2f(Config::TILE_RADIUS, Config::TILE_RADIUS);
	}

	bool isHighlighted() const { return m_isHighlighted; }
	virtual void setHighlighted(bool highlighted, const sf::Color& highlightColor= sf::Color(150, 220, 150, 180));

	//void setMonster(std::shared_ptr<Monster> monster) { m_monsterRef = monster; };
	//std::shared_ptr<Monster> getMonster() const { return m_monsterRef.lock(); }
	//bool hasMonster() const { return !m_monsterRef.expired(); }
    // --- ����� ����� ---
    void clearEntity();

    BoardEntity* getEntity() const { return m_entity; }

    // ������� ������� - ������ ������ ���� (������ ���� �����)
    //Monster* getMonster() const {
    //    if (m_entity && m_entity->getType() == EntityType::Monster) {
    //        return static_cast<Monster*>(m_entity); // Downcasting ����
    //    }
    //    return nullptr;
    //}

    void setEntity(BoardEntity* entity);
    bool hasEntity() const { return m_entity != nullptr; }

    virtual bool isPassableFor(const BoardEntity* entity) const {
        return m_isPassable;
    }

    // Both require isEntityAlive() (private, below) - a dying entity (dead,
    // but still linked to its Tile so a death animation can finish - see
    // BoardEntity::isDying/isReadyForRemoval) must not be selectable as an
    // attack or special-ability target just because it hasn't visually
    // disappeared yet. Every caller (attack-target highlighting/resolution,
    // AI targeting, ally-target highlighting) already goes through these
    // two, so this one change closes that gap everywhere at once.
    bool isOccupiedByEnemy(PlayerSide mySide) const { return isEntityAlive() && m_entity->isEnemyOf(mySide); }
    bool isOccupiedByAlly(PlayerSide mySide) const { return isEntityAlive() && m_entity->isAllyOf(mySide); }

    void receiveAttackFrom(BoardEntity* attacker);

    // ��� ����� ���� - "�� ��� ���� ��� ����� �����?" (���� �-updateTileEffects)
    //bool hasDeadEntity() const { return m_entity != nullptr && !m_entity->isAlive(); }

    /*virtual bool isPassableFor(Monster* monster) const {
        return m_isPassable;
    }*/

    //void setMonster(Monster* monster) { setEntity(monster); }

    /*bool hasMonster() const {
        return m_entity != nullptr && m_entity->getType() == EntityType::Monster;
    }*/
    //virtual bool isHole() const { return false; }

    virtual void applyTileEffect() {}
protected:
    sf::CircleShape m_shape;
    bool m_isPassable;
    sf::Color m_color;
private:
	// Only used internally, by isOccupiedByEnemy/isOccupiedByAlly above -
	// no external caller needs "is my occupant alive" on its own.
	bool isEntityAlive() const { return m_entity != nullptr && m_entity->isAlive(); }

	int m_row;
	int m_q;
	bool m_isHighlighted = false;
	//std::weak_ptr<BoardEntity> m_entityRef;
	BoardEntity* m_entity = nullptr;
};