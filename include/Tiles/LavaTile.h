#pragma once
#include "Tiles/Tile.h"

class LavaTile : public Tile
{
public:
    LavaTile(int q, int row, const sf::Vector2f& position)
        : Tile(q, row, position,sf::Color(250, 180, 180, 140))
    {
    }

    virtual void applyTileEffect() override
    {
        if (hasEntity())
        {
            if (auto entity = getEntity())
                entity->takeDamage(20); // ���� ������ 20 ���� ������ ������ ����
        }
    }

    void setHighlighted(bool highlighted, const sf::Color& defaultHighlightColor = sf::Color(150, 220, 150, 180)) override
    {
        if (highlighted) {
            // �� ������ ����� �����, ���� ���� �����/���� ��� ����� ����� �����
            Tile::setHighlighted(true, sf::Color(255, 120, 80, 180));
        }
        else {
            // �� ����� �� �����, ���� ��������� ��� ������ �� ���� �-m_color ����� �� ����
            Tile::setHighlighted(false, sf::Color::Transparent);
        }
    }
private:
};