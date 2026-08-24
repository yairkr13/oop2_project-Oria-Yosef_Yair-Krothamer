#pragma once
#include "Heart.h"
#include "Tiles/Tile.h"

class PanicPoint : public Tile
{
public:
    PanicPoint(int q, int row, const sf::Vector2f& position, Heart* p1Heart, Heart* p2Heart)
        : Tile(q, row, position, sf::Color(200, 100, 250, 140)),
        m_p1Heart(p1Heart), m_p2Heart(p2Heart)
    {
    }
    void applyTileEffect() override // �������� ����� ���� ��������!
    {
        if (hasEntity())
        {
            auto entity = getEntity();

            // ������� �������� ������ ����:
            if (entity->getSide() == PlayerSide::Left && m_p2Heart)
                m_p2Heart->takeDamage(10);
            else if (entity->getSide() == PlayerSide::Right && m_p1Heart)
                m_p1Heart->takeDamage(10);
        }
    }

    void setHighlighted(bool highlighted, const sf::Color& defaultHighlightColor = sf::Color(150, 220, 150, 180)) override
    {
        if (highlighted) {
            Tile::setHighlighted(true, sf::Color(200, 100, 250, 180));
        }
        else {
            // �� ����� �� �����, ���� ��������� ��� ������ �� ���� �-m_color ����� �� ����
            Tile::setHighlighted(false, sf::Color::Transparent);
        }
    }
private:
    Heart* m_p1Heart; // ���� ����� ���� ��� �� ���� 1
    Heart* m_p2Heart;
};