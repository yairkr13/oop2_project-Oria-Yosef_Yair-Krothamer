#pragma once
#include "SFML/Graphics.hpp"
#include "Tile.h"
#include <cmath>
#include <map>
//check
class Board
{
public:
	Board();
	void draw(sf::RenderWindow& window) const;
	bool isTilePassable(Tile* start, Tile* end) const;
private:
	void createBoard();
	std::map<std::pair<int, int>, std::unique_ptr<Tile>> m_grid;
};