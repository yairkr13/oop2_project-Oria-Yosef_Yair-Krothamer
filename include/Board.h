#pragma once
#include "SFML/Graphics.hpp"
#include "Tile.h"
#include <vector>
#include "Monsters/Monster.h"
#include "Constants.h"
#include "Heart.h"
#include <cmath>
#include <map>
#include <random>

//try
class Board
{
public:
	Board();
	void draw(sf::RenderWindow& window, PlayerSide currentTurnSide) const;
	void handleClick(const sf::Vector2f& pos, PlayerSide currentSide);
	//bool isTilePassable(Tile* start, Tile* end) const;
	
	void update(float dt);
	bool isAnimating() const;

	
	//bool trySpawnMonster(const sf::Vector2f& pos, std::shared_ptr<Monster> monster);
	bool trySpawnMonster(const sf::Vector2f& pos, Monster* monster);
	void highlightSpawnTiles(PlayerSide side);
	void clearHighlights();

	void updateTileEffects();
	void initPlayerHearts(Heart* p1Heart, Heart* p2Heart);

	Tile* AI_FindBestTargetForMonster(Monster* monster);
	sf::Vector2f tileToScreen(int q, int row) const;
	bool AI_SpawnMonster(Monster* monster, PlayerSide side);
	void performAction(Monster* monster, Tile* targetTile);// פונקציית ליבה שמבצעת את הפעולה הפיזית על הלוח (משותפת לאדם ולמחשב)

	std::vector<Tile*> getReachableTiles(Monster* monster) const;
private:
	//void setHighlight(const sf::Vector2f& pos, int range);
	//sf::Vector2f tileToScreen(int q, int row) const;
	//void highlightNeighbors(int q, int row, int range);
	void highlightNeighbors(Monster* monster); // שינוי חתימה
	Tile* getLeftmostTileInRow(int row) const;
	Tile* getRightmostTileInRow(int row) const;
	//std::vector<Tile*> getReachableTiles(Monster* monster) const;

	bool spawnMonsterOnTile(Monster* monster, Tile* targetTile);
	// פונקציית ליבה שמבצעת את הפעולה הפיזית על הלוח (משותפת לאדם ולמחשב)

	void generateSpecialTiles(Heart* p1Heart, Heart* p2Heart);
	void createBoard();

	std::pair<int, int> screenToTile(const sf::Vector2f& pos) const;
	// ��� public:
	//std::vector <Monster*> m_monsters;
	std::map<std::pair<int, int>, std::unique_ptr<Tile>> m_grid;

	// Board layout constants
	static constexpr float START_X = 320.f;
	static constexpr float START_Y = 30.f;

	static std::mt19937& rng() {
		static std::mt19937 gen(std::random_device{}());
		return gen;
	}
};