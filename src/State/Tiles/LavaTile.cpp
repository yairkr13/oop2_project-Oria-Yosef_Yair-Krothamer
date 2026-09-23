#include "Tiles/LavaTile.h"

LavaTile::LavaTile(int q, int row, const sf::Vector2f& position)
    : Tile(q, row, position,sf::Color(250, 180, 180, 140))
{
}

void LavaTile::applyTileEffect()
{
    if (hasEntity())
    {
        // canFly() is read-only - the const getEntity() is enough for it.
        // Actually dealing damage goes through damageEntity() (Tile's
        // own method, same shape as tickTurnBoundary/updateEntity) -
        // getMutableEntity() is never needed here at all.
        if (getEntity()->canFly())
            return; // מעופפים לא נפגעים מלבה
        damageEntity(20); // ���� ������ 20 ���� ������ ������ ����
    }
}

std::optional<sf::Color> LavaTile::ownHighlightColor() const
{
    return sf::Color(255, 120, 80, 180);
}
