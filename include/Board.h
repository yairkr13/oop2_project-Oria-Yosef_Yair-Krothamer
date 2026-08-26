#pragma once
#include "SFML/Graphics.hpp"
#include "Tiles/Tile.h"
#include <vector>
#include "Monsters/Monster.h"
#include "Constants.h"
#include "Heart.h"
#include <cmath>
#include <map>
#include <random>
#include <algorithm>
#include <memory>
//try
class Board
{
public:
	Board();
	void draw(sf::RenderWindow& window, PlayerSide currentTurnSide) const;
	//bool isTilePassable(Tile* start, Tile* end) const;

	void update(float dt);
	bool isAnimating() const;


	//bool trySpawnMonster(const sf::Vector2f& pos, std::shared_ptr<Monster> monster);
	bool trySpawnMonster(const sf::Vector2f& pos, Monster* monster);
	void highlightSpawnTiles(PlayerSide side);
	void clearHighlights();

	void updateTileEffects();
	void initPlayerHearts(Heart* p1Heart, Heart* p2Heart); //change this funciton!!!!! 

	// AI_FindBestTargetForMonster עברה ל-AIPlayer - Board נשאר "טיפש" ומספק
	// רק שאילתות (getReachableTiles), ההחלטה איזה יעד עדיף היא לא באחריותו.
	sf::Vector2f tileToScreen(int q, int row) const;


	// באותה רוח: Board לא בוחר איפה לזמן (זו הייתה AI_SpawnMonster הישנה - גם
	// אם ה"החלטה" היא רק הגרלה, זו עדיין בחירה שלא שייכת ל-Board). הוא רק
	// מספק את רשימת המשבצות החוקיות לזימון עבור הצד הזה; AIPlayer בוחר מתוכה.
	std::vector<Tile*> getSpawnableTiles(Monster* monster, PlayerSide side) const;

	//bool AI_SpawnMonster(Monster* monster, PlayerSide side);
	void performAction(BoardEntity* entity, Tile* targetTile);// פונקציית ליבה שמבצעת את הפעולה הפיזית על הלוח (משותפת לאדם ולמחשב)

	// Validates that `entity` may currently be selected by `side` and, if
	// so, highlights its move/attack options - exactly what GameplayState
	// needs both when interpreting a direct board click on an entity, and
	// when auto-selecting a monster right after its own no-target Special
	// changes what it can do (e.g. Barzilla's Empowered Attack extending
	// his attack range). Returns whether selection actually happened, so
	// the caller (which owns the "what is currently selected" state - see
	// GameplayState::m_selectedEntity) knows whether to remember `entity`.
	// Board itself no longer tracks "who is selected" - that's interaction
	// state, not board state.
	bool selectEntity(BoardEntity* entity, PlayerSide side);

	// Highlights every Tile in `tiles` with `color` - the generic operation
	// behind highlightNeighbors/highlightSpawnTiles below, exposed so a
	// caller with its own caller-specific Tile selection (e.g. GameplayState
	// highlighting valid Special-ability targets) can ask Board to paint
	// them instead of calling Tile::setHighlighted directly. Knows nothing
	// about *why* these tiles are being highlighted - same "Board owns Tile
	// painting, never a bystander" principle highlightNeighbors already
	// follows.
	void highlightTiles(const std::vector<Tile*>& tiles, const sf::Color& color);

	// שלב א' של הפירוק: כל ה-tiles שהמפלצת יכולה להגיע/לתקוף אליהם, בלי לצייר
	// שום דבר. לוגיקה טהורה - אין כאן שום קריאה ל-setHighlighted. גם highlightNeighbors
	// (לקליק אנושי) וגם AIPlayer (בעתיד, לצורך ההיוריסטיקה) ישתמשו באותה פונקציה הזו,
	// כדי שה-BFS לא ישוכפל בשני מקומות.
	std::vector<Tile*> getReachableTiles(Monster* monster) const;

	// Enemy tiles reachable ONLY because of a monster's extended attack
	// range (Monster::getAttackRange() > getRange() - see Barzilla's
	// Empowered Attack) - i.e. beyond normal move/attack reach but still
	// within the extended reach. Reuses the exact same shared BFS as
	// getReachableTiles (see computeReachability's extra out-parameter
	// below): NOT movement-legal (never appears in getReachableTiles, so
	// performAction's movement branch already rejects them - see there),
	// purely an additional set for highlighting "this monster can strike
	// here but not stand here." Empty for every monster whose
	// getAttackRange() == getRange() (the default for all monsters except
	// an empowered Barzilla).
	std::vector<Tile*> getExtendedAttackOnlyTiles(Monster* monster) const;

	// שלב ב': אותה שאילתה, אבל מחזירה את המסלול המדורג (לפי סדר) מהמפלצת ל-target
	// הספציפי, לא רק "מה אפשר". target חייב להיות tile שכבר יצא מ-getReachableTiles
	// (כלומר תנועה, לא תקיפה) - אחרת מוחזרת רשימה ריקה.
	std::vector<Tile*> getPathTo(Monster* monster, Tile* target) const;

	bool spawnMonsterOnTile(Monster* monster, Tile* targetTile);

	// מקור רנדומליות אחד ומשותף לכל הלוח (במקום std::mt19937 מקומי במקום אחד ו-rand() במקום אחר)
	static std::mt19937& rng()
	{
		static std::mt19937 gen(std::random_device{}());
		return gen;
	}
	// Board.h - להוסיף תחת public:
	bool isSpawnPositionValid(const sf::Vector2f& pos) const;

	// Board-fact lookups reused by anything that needs "what tile is at
	// this coordinate/screen position" - e.g. Blue's knockback (coordinate
	// arithmetic -> Tile) and special-ability target selection (a screen
	// click -> Tile). Board is the Information Expert for tile occupancy;
	// these are the two genuinely-missing general primitives, not
	// duplicates of the range/reachability/occupancy queries above.
	Tile* getTileAt(int q, int row) const;
	Tile* getTileAtScreenPosition(const sf::Vector2f& pos) const;

	// Every currently-occupied Tile - a plain board-occupancy fact, exactly
	// like the queries above. This is what lets GameplayState discover
	// candidate Special-ability targets (by pairing each Tile's entity with
	// the pending monster's own isValidSpecialTarget()) without Board ever
	// needing to know ally/enemy rules, Special abilities, or highlight
	// colors - it only ever answers "what's occupied," never "what's valid."
	std::vector<Tile*> getOccupiedTiles() const;
private:
	// ה-BFS המשותף לשתי השאילתות למעלה - מעבר יחיד, לא משוכפל. בנוסף לרשימת ה-tiles
	// הנגישים (outReachable, בדיוק כמו ש-getReachableTiles מחזירה), שומר גם "מאיפה
	// הגעתי לכאן" (outParent) - כדי ש-getPathTo תוכל לשחזר את המסלול המלא בלי להריץ
	// שוב את כל בדיקות החסימה.
	// outExtendedAttackOnly (optional): when non-null, tiles found beyond
	// monster->getRange() but within monster->getAttackRange() are recorded
	// here instead of outReachable/outParent - reached by continuing the
	// SAME walk (still respecting the usual blocking/passability rules),
	// just not counted as movement-legal or path-able-to. Every existing
	// caller passes nullptr and every monster's getAttackRange() defaults
	// to getRange(), so this is a no-op extension: behavior for them is
	// unchanged.
	void computeReachability(Monster* monster,
		std::vector<Tile*>& outReachable,
		std::map<std::pair<int, int>, std::pair<int, int>>& outParent,
		std::vector<Tile*>* outExtendedAttackOnly = nullptr) const;

	// The one place that answers "can this entity enter/traverse this
	// neighboring Tile" for the BFS above - the natural extension point for
	// any future movement/collision rule. Returns whether `monster` can
	// continue walking THROUGH `tile` (i.e. computeReachability's frontier
	// management should mark it visited and, if unoccupied, keep exploring
	// past it); false means it blocks further movement. Either way, also
	// records `tile` into outReachable/outParent/outExtendedAttackOnly via
	// recordReachability() below when it falls within range - movement
	// legality and attack-range recording are evaluated together here
	// because both depend on the same passability/occupancy facts about
	// this one neighbor.
	bool visitNeighbor(Monster* monster, Tile* tile,
		const std::pair<int, int>& neighbor, const std::pair<int, int>& parent,
		int neighborDist, int range, int attackRange,
		std::vector<Tile*>& outReachable,
		std::map<std::pair<int, int>, std::pair<int, int>>& outParent,
		std::vector<Tile*>* outExtendedAttackOnly) const;

	// Records `tile` into outReachable (+ outParent, so getPathTo can later
	// reconstruct a route to it) when within `range`, or into
	// outExtendedAttackOnly when beyond `range` but still within
	// `attackRange` - shared by both of visitNeighbor's cases (a passable
	// tile always gets recorded this way; a blocked-but-enemy-occupied tile
	// gets recorded the same way, only when occupied by an enemy).
	static void recordReachability(Tile* tile,
		const std::pair<int, int>& neighbor, const std::pair<int, int>& parent,
		int neighborDist, int range, int attackRange,
		std::vector<Tile*>& outReachable,
		std::map<std::pair<int, int>, std::pair<int, int>>& outParent,
		std::vector<Tile*>* outExtendedAttackOnly);

	// performAction()'s two independent branches, split out so each reads
	// as one responsibility. performAttack coordinates the attack (wires an
	// animation if the attacker supplies one, otherwise resolves through
	// Tile::receiveAttackFrom immediately) - it never computes a damage
	// number or inspects concrete Monster types; that stays entirely below
	// Tile::receiveAttackFrom, inside Monster::attack(). performMove keeps
	// the existing movement coordination (reachability/path/target-tile
	// handling) unchanged.
	void performAttack(BoardEntity* entity, Tile* targetTile);
	void performMove(Monster* monster, Tile* targetTile);

	//void setHighlight(const sf::Vector2f& pos, int range);
	//sf::Vector2f tileToScreen(int q, int row) const;
	//void highlightNeighbors(int q, int row, int range);
	void highlightNeighbors(Monster* monster); // שינוי חתימה
	Tile* getLeftmostTileInRow(int row) const;
	Tile* getRightmostTileInRow(int row) const;
	std::pair<int, int> screenToTile(const sf::Vector2f& pos) const;

	//bool spawnMonsterOnTile(Monster* monster, Tile* targetTile);
	// פונקציית ליבה שמבצעת את הפעולה הפיזית על הלוח (משותפת לאדם ולמחשב)

	// The hex grid's raw base position for (q,row) - NOT the tile's visual
	// center. This is what Tile's own constructor needs: Tile's shape never
	// has its origin set (see Tile::Tile/m_shape), so SFML anchors it at
	// this raw point, Config::TILE_RADIUS short of the true center in both
	// axes. tileToScreen() below builds the center from this same base by
	// adding that correction - the single place both quantities are
	// computed from, so Tile construction and tileToScreen's center can
	// never drift apart the way they did before this existed.
	sf::Vector2f tileAnchor(int q, int row) const;

	// Shared creation/replacement mechanics behind generateSpecialTiles()
	// below: builds TileType at `coords`'s raw anchor position (see
	// tileAnchor above - NOT tileToScreen's center, which is the wrong
	// quantity for constructing a Tile) and swaps it into m_grid. TileType
	// is always known at the call site (never chosen at runtime), and
	// constructor argument lists genuinely differ between Tile subtypes
	// (PanicPoint needs both Hearts; Hole/LavaTile need neither) - Args...
	// forwards whatever extra constructor arguments TileType needs beyond
	// (q, row, position). Declared here (a private member template needs to
	// be), but defined entirely in Board.cpp - its only caller - since
	// nothing outside this file ever needs to see the template body.
	template <typename TileType, typename... Args>
	void createSpecialTile(const std::pair<int, int>& coords, Args&&... extraArgs);

	void generateSpecialTiles(Heart* p1Heart, Heart* p2Heart);
	void createBoard();

	
	// ��� public:
	//std::vector <Monster*> m_monsters;
	std::map<std::pair<int, int>, std::unique_ptr<Tile>> m_grid;

	// Board layout constants
	static constexpr float START_X = 320.f;
	static constexpr float START_Y = 30.f;
};