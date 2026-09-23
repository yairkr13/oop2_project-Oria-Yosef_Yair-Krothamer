#pragma once
#include "Tiles/Tile.h"

class Hole : public Tile
{
public:
    Hole(int q, int row, const sf::Vector2f& position);
    //bool isPassableFor(Monster* monster) const override
    //{
    //    if (monster && monster->canFly()) {
    //        return true; // ������� ������ �����!
    //    }
    //    return false; // ������ ���� ������
    //}
    bool isPassableFor(const BoardEntity* entity) const override;

    //bool isHole() const override { return true; } // ���� ������� ��������� ������ ������ �-Tile.h
protected:
    std::optional<sf::Color> ownHighlightColor() const override;
};