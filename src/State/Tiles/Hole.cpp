#include "Tiles/Hole.h"

Hole::Hole(int q, int row, const sf::Vector2f& position)
    : Tile(q, row, position, sf::Color(30, 30, 30, 200)) // ��� ��� ����, ���� ��� ����
{
    // �� �� ����! ���� ������ �-Pathfinding ����� ����� ����
    m_isPassable = false;
}

bool Hole::isPassableFor(const BoardEntity* entity) const
{
    return entity && entity->canFly();
}

std::optional<sf::Color> Hole::ownHighlightColor() const
{
    return sf::Color(30, 30, 30, 200);
}
