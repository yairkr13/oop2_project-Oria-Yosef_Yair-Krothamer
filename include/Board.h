#pragma once
#include "SFML/Graphics.hpp"
#include "Tile.h"
#include <map>

class Board
{
public:
	Board();
	void draw(sf::RenderWindow& window) const;
	bool isTilePassable(Tile* start, Tile* end) const;
private:
	std::map<std::pair<int, int>, std::unique_ptr<Tile>> m_grid;
};