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
	void handleClick(const sf::Vector2f& pos, PlayerSide currentSide);
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

	// Selects `entity` (on behalf of `side`) and highlights its move/attack
	// options - exactly what a direct board click on it already does (see
	// handleClick's own "select a new entity" branch, which now calls this
	// too, so the logic exists in exactly one place). Extracted so any other
	// caller needing "select this and show its current options" reuses the
	// identical flow instead of re-deriving it - e.g. GameplayState
	// auto-selecting a monster right after its own no-target Special changes
	// what it can do (Barzilla's Empowered Attack extending his attack
	// range). Deselects whatever else might already be selected first, so
	// calling this from outside a board click can never leave two entities
	// marked selected at once.
	void selectEntity(BoardEntity* entity, PlayerSide side);

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

	//void setHighlight(const sf::Vector2f& pos, int range);
	//sf::Vector2f tileToScreen(int q, int row) const;
	//void highlightNeighbors(int q, int row, int range);
	void highlightNeighbors(Monster* monster); // שינוי חתימה
	Tile* getLeftmostTileInRow(int row) const;
	Tile* getRightmostTileInRow(int row) const;
	std::pair<int, int> screenToTile(const sf::Vector2f& pos) const;

	//bool spawnMonsterOnTile(Monster* monster, Tile* targetTile);
	// פונקציית ליבה שמבצעת את הפעולה הפיזית על הלוח (משותפת לאדם ולמחשב)

	void generateSpecialTiles(Heart* p1Heart, Heart* p2Heart);
	void createBoard();

	
	// ��� public:
	//std::vector <Monster*> m_monsters;
	std::map<std::pair<int, int>, std::unique_ptr<Tile>> m_grid;

	// Board layout constants
	static constexpr float START_X = 320.f;
	static constexpr float START_Y = 30.f;
};