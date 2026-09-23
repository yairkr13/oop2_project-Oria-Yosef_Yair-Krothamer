#include "BoardEntity.h"
#include "Tiles/Tile.h"
#include "Attacks/AttackAnimation.h" // complete type needed for the unique_ptr default below
#include <algorithm>
#include "SoundPlayer.h"

void BoardEntity::spawnOnBoard(int q, int row, const sf::Vector2f& screenPos) {
    m_q = q;
    m_row = row;
    m_screenPos = screenPos;
}

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
