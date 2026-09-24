#pragma once
#include "SFML/Graphics.hpp"
#include "Tiles/Tile.h"
#include <vector>
#include "Monsters/Monster.h"
#include "Constants.h"
#include "Heart.h"
#include "BoardPathfinder.h"
#include "BoardGenerator.h"
#include <cmath>
#include <map>
#include <random>
#include <algorithm>
#include <memory>
#include <utility>

// Owns the hex grid of Tiles, and is the single place that spawns, moves,
// attacks and highlights entities on it. Also answers reachability/geometry
// queries (getReachableTiles, tileToScreen, ...) for GameplayState/AIPlayer -
// they decide what to do, Board only ever executes it or reports facts.
class Board
{
public:
    explicit Board(const BoardLayout& layout = BoardGenerator::standardLayout());
    void draw(sf::RenderWindow& window, PlayerSide currentSide) const;

    void update(float dt) const;
    // True if any occupied tile's entity is currently animating.
    bool isAnimating() const;

    void highlightSpawnTiles( PlayerSide side) const;
    void clearHighlights() const;

    void updateTileEffects() const;
    // useFixedSpecialTiles (PlayerVsRemote only) skips the RNG and places
    // special tiles from a hardcoded layout, so both peers land on the same board.
    void initPlayerHearts(Heart* p1Heart, Heart* p2Heart, bool useFixedSpecialTiles = false);

    sf::Vector2f tileToScreen(int q, int row) const;

    std::vector<const Tile*> getSpawnableTiles(const Monster* monster, PlayerSide side) const;

    void performAction(BoardEntity* entity,const Tile* targetTile) const;

    // Checks entity can be selected by side and, if so, highlights its
    // move/attack options. Returns whether selection happened.
    bool selectEntity(const BoardEntity* entity, PlayerSide side) const;

    // includeAllies (default false) also returns ally-occupied tiles, needed
    // for Special-ability target search.
    std::vector<const Tile*> getReachableTiles(const BoardEntity* entity, bool includeAllies = false) const;

    // Same query, narrowed to reachable tiles that are actually occupied.
    std::vector<const Tile*> getReachableOccupiedTiles(const BoardEntity* entity, bool includeAllies = false) const;

    bool spawnEntityOnTile(BoardEntity* entity,const Tile* targetTile) const;

    // Picks one tile uniformly at random (Board's own shared rng()); nullptr for an empty list.
    const Tile* pickRandomTile(const std::vector<const Tile*>& tiles) const;

    int getMiddleRow() const;
    const Tile* getExtremeTileInRow(int row, bool findLeftmost) const;
    const Tile* getTileAtScreenPosition(const sf::Vector2f& pos) const;
    const Tile* getTileAt(int q, int row) const;

    void highlightValidSpecialTargets(const Monster* caster) const;
    // Pushes entity up to maxTiles steps in direction (dq, dr), stopping at
    // the first blocked/occupied/off-board tile. maxTiles is the caller's
    // own ability balance value, never hardcoded here.
    void applyKnockback(BoardEntity* entity, int dq, int dr, int maxTiles) const;
private:
    Tile* getMutableTileAt(int q, int row) const;
    void highlightNeighbors(const BoardEntity* entity) const;

    // Only performMove calls this - no external caller asks Board for a path directly.
    std::vector<const Tile*> getPathTo(const BoardEntity* entity, const Tile* target) const;

    // One shared RNG for the whole board's lifetime.
    static std::mt19937& rng();

    // Generic "paint these tiles this color" primitive shared by every highlight* method.
    void highlightTiles(const std::vector<const Tile*>& tiles, const sf::Color& color = sf::Color(150, 220, 150, 180)) const; // default: light green

    // The [minQ, maxQ] column band reserved for side's spawns.
    std::pair<int, int> spawnColumnRange(PlayerSide side) const;
    // performAction()'s two branches, split so each reads as one responsibility.
    void performAttack(BoardEntity* entity, Tile* targetTile) const;
    void performMove(BoardEntity* entity, Tile* targetTile) const;

    std::pair<int, int> screenToTile(const sf::Vector2f& pos) const;

    // The hex grid's raw base position for (q,row) - NOT the tile's visual
    // center (Tile's shape has no origin set). tileToScreen() adds the
    // TILE_RADIUS correction to get the true center.
    sf::Vector2f tileAnchor(int q, int row) const;

    // Thin wrappers around BoardGenerator: createBoard() builds the plain
    // grid at construction; generateSpecialTiles() swaps in special tiles
    // once Hearts exist.
    void generateSpecialTiles(Heart* p1Heart, Heart* p2Heart, bool useFixedPlacement);
    void createBoard();

    // This board's own layout recipe (shape + special-tile plan).
    BoardLayout m_layout;

    std::map<std::pair<int, int>, std::unique_ptr<Tile>> m_grid;

    // Owns the reachability/pathfinding BFS. Must be declared after m_grid -
    // it binds to it by reference at construction.
    BoardPathfinder m_pathfinder;

    // The board's own on-screen anchor - computed once in the constructor
    // from the actual layout, so the board stays centered for whatever
    // shape m_layout turns out to be.
    float m_startX;
    float m_startY;
};
