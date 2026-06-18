#pragma once
#include "SFML/Graphics.hpp"
#include "Tile.h"
#include <vector>
#include "Monster.h"
#include <cmath>
#include <map>

class Board
{
public:
	Board();
	void draw(sf::RenderWindow& window) const;
	void handleClick(const sf::Vector2f& pos);
	bool isTilePassable(Tile* start, Tile* end) const;
private:
	void setHighlight(const sf::Vector2f& pos, int range);
	void createBoard();
	std::vector <std::unique_ptr<Monster>> m_monsters;
	std::map<std::pair<int, int>, std::unique_ptr<Tile>> m_grid;
};