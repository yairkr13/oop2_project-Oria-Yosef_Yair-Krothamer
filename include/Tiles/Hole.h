#pragma once
#include "Tiles/Tile.h"

class Hole : public Tile
{
public:
    Hole(int q, int row, const sf::Vector2f& position)
        : Tile(q, row, position, sf::Color(30, 30, 30, 200)) // ��� ��� ����, ���� ��� ����
    {
        // �� �� ����! ���� ������ �-Pathfinding ����� ����� ����
        m_isPassable = false;
    }
    bool isPassableFor(const BoardEntity* entity) const override {
        return entity && entity->canFly();
    }
protected:
    std::optional<sf::Color> ownHighlightColor() const override { return sf::Color(30, 30, 30, 200); }
};