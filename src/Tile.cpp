#include "Tile.h"
const float TILE_RADIUS = 48.f;// Assuming each tile is 64x64 pixels, the radius would be 32


Tile::Tile(int q, int row, const sf::Vector2f& position)// <--- ��� ����� ������ ����� �� ����� ���!
    :m_q(q),
    m_row(row),
    m_isPassable(true)
{
    // ������ ������ (m_shape)...

    m_shape.setRadius(TILE_RADIUS);

    // ���� ����� ���: ���� �� 6 ������ ��� �����!
    m_shape.setPointCount(6);

    // ��� �����
    m_shape.setFillColor(sf::Color(180, 180, 180, 140)); // light gray, semi-transparent

    m_shape.setOutlineThickness(2.f);
    m_shape.setOutlineColor(sf::Color(80, 80, 80, 180));

    // ������ �� ������
    //m_shape.setRotation(sf::degrees(30.f));
    m_shape.setPosition(position);
    //m_shape.setOrigin({ TILE_RADIUS, TILE_RADIUS });
    // ���������: ������ �� ����� ����� ����� ������
    // m_shape.setOrigin(radius, radius);
}

void Tile::draw(sf::RenderWindow& window) const
{
    window.draw(m_shape);
}

void Tile::setHighlighted(bool highlighted)
{
    m_isHighlighted = highlighted;
    if (highlighted)
    {
        m_shape.setFillColor(sf::Color(150, 220, 150, 180)); // light green, semi-transparent
        m_shape.setOutlineColor(sf::Color(200, 255, 200, 220));
    }
    else
    {
        m_shape.setFillColor(sf::Color(180, 180, 180, 140)); // light gray, semi-transparent
        m_shape.setOutlineColor(sf::Color(80, 80, 80, 180));
    }
}