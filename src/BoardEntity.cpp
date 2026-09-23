#include "BoardEntity.h"
#include "Tiles/Tile.h"
#include "Attacks/AttackAnimation.h" // complete type needed for the unique_ptr default below
#include <algorithm>
#include "SoundPlayer.h"

BoardEntity::BoardEntity(int q, int row, const sf::Vector2f& position, int health)
    : m_q(q), m_row(row), m_screenPos(position), m_health(health), m_maxHealth(health) {
}

int BoardEntity::getMaxHealth() const { return m_maxHealth; }

int BoardEntity::getHealth() const { return m_health; }

bool BoardEntity::isEnemyOf(PlayerSide otherSide) const { return getSide() != otherSide; }
bool BoardEntity::isAllyOf(PlayerSide otherSide) const { return getSide() == otherSide; }
void BoardEntity::attack(BoardEntity* target) {}
int BoardEntity::getAttackRange() const { return getRange(); }
int BoardEntity::getRange() const { return 0; }

bool BoardEntity::isProtected() const { return m_protected; }

bool BoardEntity::canBeSelectedBy(PlayerSide side) const { return false; }

//virtual bool isSelected() const { return false; }
bool BoardEntity::isMoving() const { return false; }

// True while this entity is playing its own attack animation (see
// createAttackAnimation/playAttackAnimation below).
bool BoardEntity::isAttacking() const { return false; }

bool BoardEntity::isUsingSpecialAnimation() const { return false; }

bool BoardEntity::isDying() const { return false; }

bool BoardEntity::isAnimating() const { return isMoving() || isAttacking() || isUsingSpecialAnimation() || isDying(); }

bool BoardEntity::isReadyForRemoval() const { return !isAlive(); }

int BoardEntity::getQ() const { return m_q; }
int BoardEntity::getRow() const { return m_row; }
void BoardEntity::setCoords(int q, int row) { m_q = q; m_row = row; }

void BoardEntity::spawnOnBoard(int q, int row, const sf::Vector2f& screenPos) {
    m_q = q;
    m_row = row;
    m_screenPos = screenPos;
}
void BoardEntity::update(float dt) {}
bool BoardEntity::canFly() const { return false; }
sf::Vector2f BoardEntity::getScreenPosition() const { return m_screenPos; }
//virtual Monster* asMonster() { return nullptr; }
bool BoardEntity::canBeTargetedBySpecial() const { return false; }

std::unique_ptr<AttackAnimation> BoardEntity::createAttackAnimation(sf::Vector2f targetPosition) const
{
    return nullptr;
}

void BoardEntity::playAttackAnimation(std::unique_ptr<AttackAnimation> animation)
{
    // No-op default: entities that never produce an animation never receive one here either.
}

void BoardEntity::playSpecialAbilityAnimation(std::unique_ptr<AttackAnimation> animation)
{
    // No-op default, same reasoning as playAttackAnimation above.
}

bool BoardEntity::isAlive() const
{
    return m_health > 0;
}

void BoardEntity::takeDamage(int damage)
{
    if (m_protected) return; // encapsulated here so no caller ever needs to ask first
    m_health -= damage;
    if (m_health <= 0)
    {
        SoundPlayer::getInstance().play("dead_sound");
        m_health = 0;
    }
}

void BoardEntity::heal(int amount)
{
    m_health = std::min(m_health + amount, m_maxHealth);
}

void BoardEntity::applyProtection()
{
    m_protected = true;
    // Survives the switch away from the caster's turn, then the whole
    // opposing turn, expiring on the switch back - see onTurnBoundary().
    m_protectionTurnsRemaining = 2;
}

void BoardEntity::onTurnBoundary()
{
    if (m_protectionTurnsRemaining > 0)
    {
        --m_protectionTurnsRemaining;
        if (m_protectionTurnsRemaining == 0)
            m_protected = false;
    }
}


void BoardEntity::drawHealthBar(sf::RenderWindow& window) const
{
    if (!isAlive()) return;
    /*if (m_side == currentPlayer)
        return;*/

    float barWidth = Config::MONSTER_BOARD_SIZE * 0.65f;
    float barHeight = 6.f;

    float x = m_screenPos.x - (barWidth / 2.f);
    float y = m_screenPos.y - (Config::MONSTER_BOARD_SIZE / 2.f) - 10.f;

    sf::RectangleShape bgBar({ barWidth, barHeight });
    bgBar.setPosition({ x, y });
    bgBar.setFillColor(sf::Color(80, 20, 20));

    float healthRatio = static_cast<float>(m_health) / m_maxHealth;
    if (healthRatio < 0.f) healthRatio = 0.f;

    sf::RectangleShape fgBar({ barWidth * healthRatio, barHeight });
    fgBar.setPosition({ x, y });
    fgBar.setFillColor(sf::Color(50, 220, 50));

    window.draw(bgBar);
    window.draw(fgBar);
}

float BoardEntity::scoreAsAttackTarget() const { return -static_cast<float>(getHealth()); }

bool BoardEntity::canMove() const { return false; }
void BoardEntity::applyFreeze() {}

void BoardEntity::applyEmpoweredAttack(float multiplier) {}
void BoardEntity::moveAlongPath(int finalQ, int finalRow, const std::vector<sf::Vector2f>& pathScreenPositions) { /* No-op by default */ }
