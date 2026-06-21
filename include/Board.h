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
	//bool isTilePassable(Tile* start, Tile* end) const;
	
	// Board borrows monster pointers (non-owning) from Players
	//void setMonsters(std::vector<Monster*> monsters);
	//void addMonster(Monster* monster);
	bool trySpawnMonster(const sf::Vector2f& pos, std::shared_ptr<Monster> monster);

private:
	//void setHighlight(const sf::Vector2f& pos, int range);
	sf::Vector2f tileToScreen(int q, int row) const;
	void highlightNeighbors(int q, int row, int range);
	void clearHighlights();

	void createBoard();
	// תחת public:
	//std::vector <Monster*> m_monsters;
	std::map<std::pair<int, int>, std::unique_ptr<Tile>> m_grid;

	// Board layout constants
	static constexpr float TILE_RADIUS = 48.f;
	static constexpr float START_X = 320.f;
	static constexpr float START_Y = 30.f;
};