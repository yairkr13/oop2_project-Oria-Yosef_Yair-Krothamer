#include "BoardEntity.h"
#include "Tile.h" // �-include ��� ���� ���!
#include "Attacks/AttackAnimation.h" // complete type needed for the unique_ptr default below
//
//BoardEntity::~BoardEntity() {
//     ��� �-OOP ��������: 
//     ��� ���� ������ ����� ����� ��������, �� ��� ����� �� �����...
//    if (m_currentTile != nullptr) {
//         ��� ���� �� ���� ������� ��� ��� ����� �� Dangling Pointer
//        m_currentTile->clearEntity();
//    }
//}

//BoardEntity::~BoardEntity() 
//{
//    if (m_currentTile != nullptr && m_currentTile->getEntity() == this)
//        m_currentTile->clearEntity();
//    //delete this;
//}

void BoardEntity::spawnOnBoard(int q, int row, const sf::Vector2f& screenPos) {
    m_q = q;
    m_row = row;
    m_screenPos = screenPos;
}

std::unique_ptr<AttackAnimation> BoardEntity::createAttackAnimation(BoardEntity* target) const
{
    return nullptr;
}

bool BoardEntity::isAlive() const
{
    return m_health > 0;
}

void BoardEntity::takeDamage(int damage)
{
    m_health -= damage;
    if (m_health < 0) m_health = 0;
}


void BoardEntity::drawHealthBar(sf::RenderWindow& window) const
{
    if (!isAlive()) return;
    /*if (m_side == currentPlayer)
        return;*/
        // 1. ����� ����� ��� ����� (����, 80% ����� ����� ����)
    float barWidth = Config::MONSTER_BOARD_SIZE * 0.8f;
    float barHeight = 6.f;

    // 2. ����� ����� ��� ��� ������ (����� �-m_screenPos ��� ���� �����)
    float x = m_screenPos.x - (barWidth / 2.f);
    float y = m_screenPos.y - (Config::MONSTER_BOARD_SIZE / 2.f) - 12.f; // 12 ������� ��� �����

    // 3. ���� ��� �� ����� (���� ��� �� ����)
    sf::RectangleShape bgBar({ barWidth, barHeight });
    bgBar.setPosition({ x, y });
    bgBar.setFillColor(sf::Color(80, 20, 20)); // ���� ����

    // 4. ����� ��� ����� ������ ����� ��� �����
    float healthRatio = static_cast<float>(m_health) / m_maxHealth;
    if (healthRatio < 0.f) healthRatio = 0.f;

    sf::RectangleShape fgBar({ barWidth * healthRatio, barHeight });
    fgBar.setPosition({ x, y });
    fgBar.setFillColor(sf::Color(50, 220, 50)); // ���� ����

    // 5. ����� �� ����
    window.draw(bgBar);
    window.draw(fgBar);
}