#pragma once
#include "SFML/Graphics.hpp"
#include "Monsters/Monster.h"
#include "BoardEntity.h"
#include "Constants.h"
#include <memory>
//#include "StaticObject.h"

class Tile //: public StaticObject
{
public:
	Tile(int q, int row, const sf::Vector2f& position, const sf::Color& color= sf::Color(80, 80, 80, 180));
    virtual ~Tile() = default; // 1. ���� ���� ����������� ������ �����!
	void draw(sf::RenderWindow& window) const;

	// Draws only this tile's occupant (if any), never the hex itself. Split
	// out from draw() so Board can paint every tile's hex first and every
	// tile's occupant second, in two separate full passes - see Board::draw
	// for why: drawing them tile-by-tile (hex+occupant, hex+occupant, ...)
	// made layering depend on m_grid's iteration order (sorted by q), so a
	// monster mid-walk got another tile's hex painted on top of it whenever
	// that tile's q was greater than the tile the monster is logically
	// standing on - most visible moving right-to-left, since the target
	// tile (drawn early, smaller q) is where the still-mid-animation moving
	// entity is drawn from, while the tiles it's still passing over
	// (larger q) get painted after it.
	void drawEntity(sf::RenderWindow& window, PlayerSide currentSide) const;

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

    // Safe default: read-only access to whatever occupies this tile. Most
    // callers only ever need to ask the entity something (isValidSpecialTarget,
    // scoreAsAttackTarget, selectEntity's own validity check...) - none of
    // that needs a mutable pointer.
    const BoardEntity* getEntity() const { return m_entity; }

    // Mutable access - only for the few callers that actually need to
    // change the entity itself (e.g. Monster::useSpecialAbility's target).
    // Kept public (unlike Board's own private getMutableTileAt) because the
    // callers that need this - AIPlayer, GameplayState - are genuinely
    // outside Tile, not an internal implementation detail the way Board's
    // own tile mutation is. Naming it separately from getEntity() at least
    // makes every such call site say, at the call site itself, "I intend to
    // mutate this" instead of leaving that ambiguous.
    BoardEntity* getMutableEntity() const { return m_entity; }

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

    // Three more Tile-performs-an-operation-on-its-own-entity methods, same
    // shape as receiveAttackFrom above - each replaces a Board.cpp call site
    // that used to do "if (auto entity = tile->getEntity()) { entity->X();
    // if (entity->isReadyForRemoval()) tile->clearEntity(); }" itself. Board
    // no longer needs getEntity() for these three (see Board::update/
    // updateTileEffects/isAnimating) - it just asks the Tile to do it.

    // Per-turn-boundary tick (see BoardEntity::onTurnBoundary) - a no-op if
    // this tile is empty.
    void tickTurnBoundary();

    // Per-frame update (movement/attack/special animations - see
    // BoardEntity::update) - a no-op if this tile is empty.
    void updateEntity(float dt);

    // Whether this tile's own entity is currently animating (see
    // BoardEntity::isAnimating) - false if this tile is empty.
    bool isEntityAnimating() const;

    // Deals `amount` damage to this tile's own entity and cleans up if that
    // kills it (see receiveAttackFrom above - same shape, just an
    // environmental source of damage instead of an attacker). A LavaTile
    // uses this instead of getMutableEntity() + takeDamage() itself.
    void damageEntity(int amount);

    // Forwards to this tile's own entity's own scoreAsAttackTarget() (see
    // Monster/Heart's own overrides) - -infinity (never worth attacking) if
    // this tile is empty. Lets AIPlayer rank reachable enemies without ever
    // calling getEntity() itself for this one purely informational query.
    float scoreAsAttackTarget() const;

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
	BoardEntity* m_entity = nullptr;

	//std::weak_ptr<BoardEntity> m_entityRef;
};