#pragma once
#include <SFML/Graphics.hpp>
#include "Constants.h"
#include <memory>
class Monster; // Forward declaration

enum class EntityType {
    Monster,
    Heart
};

class Tile; // Forward declaration
class AttackAnimation; // Forward declaration

class BoardEntity {
public:
    BoardEntity(int q, int row, const sf::Vector2f& position, int health)
        : m_q(q), m_row(row), m_screenPos(position), m_health(health), m_maxHealth(health)/*, m_currentTile(nullptr)*/ {
    }

    // ����� �����: ���������� ����� �������� ��� �� default, ���� ���� �-cpp
    virtual ~BoardEntity() = default;

    //virtual int getRange() const { return 0; }
    // ����� ������� ������ ����:
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
    //virtual bool isSelectable() const { return false; } // ����� ����: �� �� ���� �����
    virtual bool canBeSelectedBy(PlayerSide side) const { return false; }

    //virtual bool isSelected() const { return false; }
    virtual bool isMoving() const { return false; }

    // True while this entity is playing its own attack animation (see
    // createAttackAnimation/playAttackAnimation below). Board aggregates
    // this alongside isMoving() in isAnimating() - it never needs to know
    // *which* entity or *which* animation, only whether one is in flight.
    virtual bool isAttacking() const { return false; }
    //// --- �������� �����: ��� ��-������ �� ������ ---
    //void setCurrentTile(Tile* tile) { m_currentTile = tile; }
    //Tile* getCurrentTile() const { return m_currentTile; }
    void setCurrentTile(Tile* tile) { m_currentTile = tile; }
    Tile* getCurrentTile() const { return m_currentTile; }


    // --- �������� ��� ������ (����� ���� ���� ������� ����) ---
    int getQ() const { return m_q; }
    int getRow() const { return m_row; }
    void setCoords(int q, int row) { m_q = q; m_row = row; }

    virtual void spawnOnBoard(int q, int row, const sf::Vector2f& screenPos);
    virtual void update(float dt) {}
    virtual bool canFly() const { return false; }
    sf::Vector2f getScreenPosition() const { return m_screenPos; }
    virtual Monster* asMonster() { return nullptr; }

    // Optional visual for this entity's attack: nullptr (the default, used
    // by every entity that doesn't override this) means "no animation" -
    // Board falls back to resolving the attack immediately, exactly as it
    // does today. An entity that wants an animated attack (see Mozzy)
    // overrides this to return one instead; Board never needs to know which
    // concrete entity - or which concrete animation - it got back.
    // (Defined out-of-line in BoardEntity.cpp: AttackAnimation is only
    // forward-declared here, and MSVC instantiates unique_ptr<T>'s
    // destructor at the point an inline body is defined, which needs T
    // complete even for a body as simple as `return nullptr;`.)
    virtual std::unique_ptr<AttackAnimation> createAttackAnimation(BoardEntity* target) const;

    // Hands ownership of an in-flight attack animation to this entity - it
    // owns/updates/draws it from here on, exactly like a Monster already
    // owns its own movement animation. Default is a no-op (the animation is
    // simply dropped): only entities that can attack (Monster) override
    // this; Board calls it polymorphically without knowing that. (Also
    // defined out-of-line - same incomplete-type reason as above: the
    // parameter itself is destroyed at the end of the default body.)
    virtual void playAttackAnimation(std::unique_ptr<AttackAnimation> animation);

protected:
    void drawHealthBar(sf::RenderWindow& window) const;
    // ������� ������ ��� ��� ����� ��� �� ����� ���� ����� ������ ����� �����
    int m_q;
    int m_row;
    sf::Vector2f m_screenPos;
    int m_health;
    int m_maxHealth;
    Tile* m_currentTile;
    bool m_isSelected = false;
};