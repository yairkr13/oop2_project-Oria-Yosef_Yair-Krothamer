#pragma once
#include <SFML/Graphics.hpp>
#include "Constants.h"
#include <memory>
class Monster; // Forward declaration

class Tile; // Forward declaration
class AttackAnimation; // Forward declaration

// Base class for anything that can occupy a board Tile (Monster, Heart).
// Owns position, health and the animation/attack-state predicates Board and
// Tile query polymorphically, without needing to know the concrete subtype.
class BoardEntity {
public:
    BoardEntity(int q, int row, const sf::Vector2f& position, int health)
        : m_q(q), m_row(row), m_screenPos(position), m_health(health), m_maxHealth(health) {
    }

    virtual ~BoardEntity() = default;

    virtual void draw(sf::RenderWindow& window, PlayerSide currentSide) const = 0;
    virtual void takeDamage(int damage);

    // Heals this entity, capped at max HP - the other half of the
    // encapsulated health API alongside takeDamage.
    void heal(int amount);
    int getMaxHealth() const { return m_maxHealth; }
    int getHealth() const { return m_health; }

    virtual bool isAlive() const;
    virtual PlayerSide getSide() const = 0;
    bool isEnemyOf(PlayerSide otherSide) const { return getSide() != otherSide; }
    bool isAllyOf(PlayerSide otherSide) const { return getSide() == otherSide; }
    virtual void attack(BoardEntity* target) {}
    virtual int getAttackRange() const { return getRange(); }
    virtual int getRange() const { return 0; }

    // "Cannot currently receive damage" (Henrietta's Protection). Checked
    // inside takeDamage() itself, so every caller respects it automatically.
    bool isProtected() const { return m_protected; }
    void applyProtection();

    // Per-turn-boundary tick for turn-scoped status state (currently:
    // Protection's countdown). Called once per player switch from
    // Board::updateTileEffects().
    virtual void onTurnBoundary();
    virtual bool canBeSelectedBy(PlayerSide side) const { return false; }

    virtual bool isMoving() const { return false; }

    // True while playing this entity's own attack animation.
    virtual bool isAttacking() const { return false; }

    // True while playing an incoming Special Ability's visual effect on this
    // entity (e.g. Muffintop's Heal effect on the healed ally). Kept separate
    // from isAttacking() - a passive recipient isn't attacking or attacked.
    virtual bool isUsingSpecialAnimation() const { return false; }

    // True while dead but still occupying its Tile so a death animation can
    // finish (see Monster's Die sprite sheet). False by default.
    virtual bool isDying() const { return false; }

    // Aggregate "is this entity currently doing anything visual that should
    // block the game" - callers never need to know why.
    virtual bool isAnimating() const { return isMoving() || isAttacking() || isUsingSpecialAnimation() || isDying(); }

    // Whether it's safe to clear this entity from its Tile now. Default:
    // as soon as it's dead; Monster overrides to also wait for its death animation.
    virtual bool isReadyForRemoval() const { return !isAlive(); }

    int getQ() const { return m_q; }
    int getRow() const { return m_row; }
    void setCoords(int q, int row) { m_q = q; m_row = row; }

    virtual void spawnOnBoard(int q, int row, const sf::Vector2f& screenPos);
    virtual void update(float dt) {}
    virtual bool canFly() const { return false; }
    sf::Vector2f getScreenPosition() const { return m_screenPos; }
    virtual bool canBeTargetedBySpecial() const { return false; }

    // Optional attack visual: nullptr (default) means Board resolves the
    // attack immediately. An override (e.g. Mozzy) returns one instead.
    // Takes the target's screen position, not the target entity itself.
    // (Defined out-of-line: AttackAnimation is only forward-declared here.)
    virtual std::unique_ptr<AttackAnimation> createAttackAnimation(sf::Vector2f targetPosition) const;

    // Hands ownership of an in-flight attack animation to this entity, which
    // owns/updates/draws it from here on. Default is a no-op.
    // (Also defined out-of-line - same incomplete-type reason as above.)
    virtual void playAttackAnimation(std::unique_ptr<AttackAnimation> animation);

    // Hands ownership of a Special Ability's visual effect to this entity -
    // symmetric with playAttackAnimation, but a separate slot (see isUsingSpecialAnimation).
    virtual void playSpecialAbilityAnimation(std::unique_ptr<AttackAnimation> animation);

    // How valuable this entity is as an AI attack target - higher ranks
    // higher. Default prefers low current HP; Heart overrides to always win.
    virtual float scoreAsAttackTarget() const { return -static_cast<float>(getHealth()); }

    virtual bool canMove() const { return false; }
    virtual void applyFreeze() {}

    // Grants "next attack deals damage * multiplier" (see Monster::attack()).
    // multiplier is the caller's own ability balance value, never hardcoded here.
    virtual void applyEmpoweredAttack(float multiplier) {}
    virtual void moveAlongPath(int finalQ, int finalRow, const std::vector<sf::Vector2f>& pathScreenPositions) { /* No-op by default */ }
protected:
    void drawHealthBar(sf::RenderWindow& window) const;
    int m_q;
    int m_row;
    sf::Vector2f m_screenPos;

private:
    // Only ever touched here - Monster/Heart go through the public accessors.
    int m_health;
    int m_maxHealth;
    bool m_protected = false;
    int m_protectionTurnsRemaining = 0;
};
