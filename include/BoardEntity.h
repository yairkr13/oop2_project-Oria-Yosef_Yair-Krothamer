#pragma once
#include <SFML/Graphics.hpp>
#include "Constants.h"
#include <memory>
class Monster; // Forward declaration
//
//enum class EntityType {
//    Monster,
//    Heart
//};

class Tile; // Forward declaration
class AttackAnimation; // Forward declaration

class BoardEntity {
public:
    BoardEntity(int q, int row, const sf::Vector2f& position, int health);//למהה?????? לחשוב על למחוק את המשבצת מפה!!!!! - הוסר, ראו m_currentTile למטה

    // ����� �����: ���������� ����� �������� ��� �� default, ���� ���� �-cpp
    virtual ~BoardEntity() = default;

    //virtual int getRange() const { return 0; }
    // ����� ������� ������ ����:
    virtual void draw(sf::RenderWindow& window, PlayerSide currentSide) const = 0;
    virtual void takeDamage(int damage);

    // Heals this entity, capped at its own max HP - mirrors takeDamage as
    // the other half of the same encapsulated health API, so callers (e.g.
    // Muffintop's heal ability) never manipulate another entity's health
    // fields directly.
    void heal(int amount);
    int getMaxHealth() const;

    // Current HP - mirrors getMaxHealth(), added so AI scoring
    // (Monster::scoreAsSpecialTarget - Heal/Protection preferring whoever's
    // hurt most) has a public way to ask "how much health does this have
    // right now" without reaching into m_health directly.
    int getHealth() const;

    virtual bool isAlive() const;
    //virtual int getHealth() const = 0;
    virtual PlayerSide getSide() const = 0;
    bool isEnemyOf(PlayerSide otherSide) const;
    bool isAllyOf(PlayerSide otherSide) const;
    virtual void attack(BoardEntity* target);
    virtual int getAttackRange() const;
    virtual int getRange() const;

    // Encapsulated "cannot currently receive damage" state (see Henrietta's
    // Protection). Deliberately checked *inside* takeDamage() itself, not
    // by callers - Tile::receiveAttackFrom, tile effects, everything that
    // already just calls takeDamage() respects this automatically, with no
    // `if (isProtected())` anywhere else in the codebase.
    bool isProtected() const;
    void applyProtection();

    // Generic per-turn-boundary tick for turn-scoped status state (currently:
    // Protection's countdown). Called once per player switch from
    // Board::updateTileEffects() - the same existing per-switch tick point
    // tile effects already use - so entities never need Board or TurnManager
    // to know their status effects exist. Entities with their own turn-scoped
    // state (see Barzilla's empowered attack) override this and chain to the
    // base implementation.
    virtual void onTurnBoundary();
    //virtual EntityType getType() const = 0;
    // 
    //virtual bool isSelectable() const { return false; } // ����� ����: �� �� ���� �����
    virtual bool canBeSelectedBy(PlayerSide side) const;

    //virtual bool isSelected() const { return false; }
    virtual bool isMoving() const;

    // True while this entity is playing its own attack animation (see
    // createAttackAnimation/playAttackAnimation below).
    virtual bool isAttacking() const;

    // True while this entity is playing an incoming Special Ability's visual
    // effect (see playSpecialAbilityAnimation below - e.g. Muffintop's Heal
    // effect playing on the healed ally). Kept as its own predicate rather
    // than folded into isAttacking(): an entity that's the passive subject
    // of a Special effect is not attacking or being attacked, and collapsing
    // the two would make isAttacking() lie about what's actually happening.
    virtual bool isUsingSpecialAnimation() const;

    // True while this entity is dead (see isAlive()) but still occupying
    // its Tile so a one-shot death animation can finish playing - see
    // Monster's Die sprite sheet. False (the default) for every entity
    // that either isn't dead or has no death animation to wait for, which
    // is exactly "removed from the board the instant it dies", the
    // behavior every entity already had before death animations existed.
    virtual bool isDying() const;

    // The single aggregate query callers like Board should use: "is this
    // entity currently doing anything visual that should block the game."
    // Callers never need to know *why* - this entity decides what counts.
    // Declared once here (not overridden per-subclass): because isMoving(),
    // isAttacking(), isUsingSpecialAnimation() and isDying() are themselves
    // virtual, this automatically picks up whatever the most-derived class
    // does for each, with zero extra code in Monster. When a new reason to
    // be busy is added later, it's added the same way - a new virtual
    // predicate here, defaulted to false, ORed into this one line - so only
    // this class (where the new state actually lives) needs editing, never
    // Board.
    virtual bool isAnimating() const;

    // Whether this entity should be cleared from its Tile right now - i.e.
    // whether it's actually safe/appropriate to stop drawing/updating it as
    // a board occupant. Default: as soon as it's dead, which is exactly the
    // immediate-removal behavior every entity had before death animations
    // existed - Heart and any Monster without a Die sheet configured keep
    // this default untouched. Monster overrides this to also wait for its
    // own death animation (see isDying() above) to finish first. Callers
    // (Tile::receiveAttackFrom, Board::updateTileEffects, Board::update)
    // ask only this - never isAlive() directly - to decide whether to call
    // Tile::clearEntity().
    virtual bool isReadyForRemoval() const;
    //// --- �������� �����: ��� ��-������ �� ������ ---
    //void setCurrentTile(Tile* tile) { m_currentTile = tile; }
    //Tile* getCurrentTile() const { return m_currentTile; }
    // Removed (kept as comments, not deleted): m_currentTile duplicated
    // m_q/m_row (already the authoritative position, always kept in sync by
    // spawnOnBoard/moveTo/moveAlongPath) with a second, separately-maintained
    // reference to the same fact - the two could only ever drift apart, never
    // add real information. Every caller that needs "which Tile is this
    // entity on" already has (or can easily get) a Board& and this entity's
    // own getQ()/getRow(), and can ask Board::getTileAt/getMutableTileAt
    // directly - the single source of truth Board already owns.
    // void setCurrentTile(Tile* tile) { m_currentTile = tile; }
    // Tile* getCurrentTile() const { return m_currentTile; }


    // --- �������� ��� ������ (����� ���� ���� ������� ����) ---
    int getQ() const;
    int getRow() const;
    void setCoords(int q, int row);

    virtual void spawnOnBoard(int q, int row, const sf::Vector2f& screenPos);
    virtual void update(float dt);
    virtual bool canFly() const;
    sf::Vector2f getScreenPosition() const;
    //virtual Monster* asMonster() { return nullptr; }
    virtual bool canBeTargetedBySpecial() const;

    // Optional visual for this entity's attack: nullptr (the default, used
    // by every entity that doesn't override this) means "no animation" -
    // Board falls back to resolving the attack immediately, exactly as it
    // does today. An entity that wants an animated attack (see Mozzy)
    // overrides this to return one instead; Board never needs to know which
    // concrete entity - or which concrete animation - it got back.
    // Takes the target's screen position, not the target entity itself -
    // every concrete override only ever needs target->getScreenPosition()
    // to build its animation, so Board doesn't need to hand out the actual
    // BoardEntity* (see Board::performAttack, which passes
    // targetTile->getScreenPosition() directly).
    // (Defined out-of-line in BoardEntity.cpp: AttackAnimation is only
    // forward-declared here, and MSVC instantiates unique_ptr<T>'s
    // destructor at the point an inline body is defined, which needs T
    // complete even for a body as simple as `return nullptr;`.)
    virtual std::unique_ptr<AttackAnimation> createAttackAnimation(sf::Vector2f targetPosition) const;

    // Hands ownership of an in-flight attack animation to this entity - it
    // owns/updates/draws it from here on, exactly like a Monster already
    // owns its own movement animation. Default is a no-op (the animation is
    // simply dropped): only entities that can attack (Monster) override
    // this; Board calls it polymorphically without knowing that. (Also
    // defined out-of-line - same incomplete-type reason as above: the
    // parameter itself is destroyed at the end of the default body.)
    virtual void playAttackAnimation(std::unique_ptr<AttackAnimation> animation);

    // Hands ownership of a Special Ability's visual effect to this entity -
    // symmetric with playAttackAnimation, but a separate slot (see
    // isUsingSpecialAnimation above for why). The caster decides what the
    // effect looks like and builds it (see Muffintop::onSpecialAbility);
    // this only ever receives it and takes over owning/updating/drawing it,
    // exactly like playAttackAnimation already does for outgoing attacks.
    // Default is a no-op, same reasoning as playAttackAnimation's default.
    virtual void playSpecialAbilityAnimation(std::unique_ptr<AttackAnimation> animation);
    //אולי אחר כך?????:
    // How valuable this entity is as an ATTACK target - used only by AI
    // (AIPlayer::findBestTarget) to rank among several reachable enemies.
    // Higher = more worth attacking. Default: prefers low current HP -
    // finishing off whoever's closest to dying is worth more than chipping
    // a healthy one. Heart overrides this (see Heart.h) to always outrank
    // any Monster's own health-based score - the AI never checks "is this a
    // Heart" itself, it only ever asks this same question of every
    // candidate and follows the highest answer.
    virtual float scoreAsAttackTarget() const;

    virtual bool canMove() const;
    virtual void applyFreeze();

    // Grants "next attack deals damage * multiplier" - see Monster::attack()
    // for where this is actually consumed. `multiplier` is the CALLER's own
    // ability balance number (e.g. Barzilla's Empowered Attack passes its
    // own 2x) - never hardcoded here, same reasoning as
    // Board::applyKnockback's own maxTiles parameter. Same shape as
    // applyFreeze() above: a no-op default here (Heart never attacks, so has
    // nothing to boost), overridden by Monster to actually store it. Used by
    // Barzilla's Empowered Attack, which targets an ALLY and grants them
    // this buff (mirrors Henrietta's Protection - the effect lives on the
    // recipient, not the caster).
    virtual void applyEmpoweredAttack(float multiplier);
    virtual void moveAlongPath(int finalQ, int finalRow, const std::vector<sf::Vector2f>& pathScreenPositions);
protected:
    void drawHealthBar(sf::RenderWindow& window) const;
    // ������� ������ ��� ��� ����� ��� �� ����� ���� ����� ������ ����� �����
    int m_q;
    int m_row;
    sf::Vector2f m_screenPos;
    // Tile* m_currentTile; - removed, see setCurrentTile/getCurrentTile above

private:
    // Only ever touched here in BoardEntity.cpp - neither Monster nor Heart
    // reaches these directly, both go through the public accessors
    // (getHealth/getMaxHealth/isProtected/heal/takeDamage/applyProtection).
    int m_health;
    int m_maxHealth;
    bool m_protected = false;
    int m_protectionTurnsRemaining = 0;
};