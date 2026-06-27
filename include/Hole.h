#pragma once
#include "Tile.h"

class Hole : public Tile
{
public:
    Hole(int q, int row, const sf::Vector2f& position)
        : Tile(q, row, position, sf::Color(30, 30, 30, 200)) // צבע כהה מאוד, נראה כמו תהום
    {
        // זה כל הקסם! מונע ממערכת ה-Pathfinding לאפשר הליכה לכאן
        m_isPassable = false;
    }
    bool isHole() const override { return true; } // דורס פונקציה וירטואלית שנצטרך להוסיף ל-Tile.h
    void setHighlighted(bool highlighted, const sf::Color& defaultHighlightColor = sf::Color(150, 220, 150, 180)) override
    {
        if (highlighted) {
            Tile::setHighlighted(true, sf::Color(30, 30, 30, 200));
        }
        else {
            // אם מכבים את ההארה, נקרא לפונקציית האם שתחזיר את הצבע ל-m_color הרגיל של הלבה
            Tile::setHighlighted(false, sf::Color::Transparent);
        }
    }
private:
};