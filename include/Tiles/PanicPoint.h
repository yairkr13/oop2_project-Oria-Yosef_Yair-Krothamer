#pragma once
#include "Heart.h"
#include "Tiles/Tile.h"

class PanicPoint : public Tile
{
public:
    PanicPoint(int q, int row, const sf::Vector2f& position, Heart* p1Heart, Heart* p2Heart);
    void applyTileEffect() override; // �������� ����� ���� ��������!

protected:
    std::optional<sf::Color> ownHighlightColor() const override;
private:
    Heart* m_p1Heart; // ���� ����� ���� ��� �� ���� 1
    Heart* m_p2Heart;
};