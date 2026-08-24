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
    //bool isPassableFor(Monster* monster) const override
    //{
    //    if (monster && monster->canFly()) {
    //        return true; // ������� ������ �����!
    //    }
    //    return false; // ������ ���� ������
    //}
    bool isPassableFor(const BoardEntity* entity) const override {
        return entity && entity->canFly();
    }

    //bool isHole() const override { return true; } // ���� ������� ��������� ������ ������ �-Tile.h
    void setHighlighted(bool highlighted, const sf::Color& defaultHighlightColor = sf::Color(150, 220, 150, 180)) override
    {
        if (highlighted) {
            Tile::setHighlighted(true, sf::Color(30, 30, 30, 200));
        }
        else {
            // �� ����� �� �����, ���� ��������� ��� ������ �� ���� �-m_color ����� �� ����
            Tile::setHighlighted(false, sf::Color::Transparent);
        }
    }
private:
};