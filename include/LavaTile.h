#pragma once
#include "Tile.h"

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
                entity->takeDamage(20); // הלבה מורידה 20 חיים למפלצת שעומדת עליה
        }
    }

    void setHighlighted(bool highlighted, const sf::Color& defaultHighlightColor = sf::Color(150, 220, 150, 180)) override
    {
        if (highlighted) {
            // אם המשבצת מוארת למעבר, נצבע אותה בכתום/אדום חזק במקום הירוק הרגיל
            Tile::setHighlighted(true, sf::Color(255, 120, 80, 180));
        }
        else {
            // אם מכבים את ההארה, נקרא לפונקציית האם שתחזיר את הצבע ל-m_color הרגיל של הלבה
            Tile::setHighlighted(false, sf::Color::Transparent);
        }
    }
private:
};