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
//try
class Board
{
public:  ///להוסיף const לכל הפונקציות הציבוריות!!!!!!
	// `layout` defaults to BoardGenerator::standardLayout() - today's one
	// and only map. A future second map/stage is a second BoardLayout
	// (see BoardGenerator.h) passed in here instead - Board's own code
	// never needs to change to support it.
	explicit Board(const BoardLayout& layout = BoardGenerator::standardLayout());
	void draw(sf::RenderWindow& window, PlayerSide currentSide) const;
	//bool isTilePassable(Tile* start, Tile* end) const;

	void update(float dt);
	bool isAnimating() const;


	//bool trySpawnMonster(const sf::Vector2f& pos, std::shared_ptr<Monster> monster);
	//bool trySpawnMonster(const sf::Vector2f& pos, Monster* monster);
	void highlightSpawnTiles( PlayerSide side);
	void clearHighlights();

	void updateTileEffects(); //למה יש לזה את השם הזה ולא את השם end turn?????
	void initPlayerHearts(Heart* p1Heart, Heart* p2Heart); //change this funciton!!!!! 

	// AI_FindBestTargetForMonster עברה ל-AIPlayer - Board נשאר "טיפש" ומספק
	// רק שאילתות (getReachableTiles), ההחלטה איזה יעד עדיף היא לא באחריותו.
	sf::Vector2f tileToScreen(int q, int row) const;


	// באותה רוח: Board לא בוחר איפה לזמן (זו הייתה AI_SpawnMonster הישנה - גם
	// אם ה"החלטה" היא רק הגרלה, זו עדיין בחירה שלא שייכת ל-Board). הוא רק
	// מספק את רשימת המשבצות החוקיות לזימון עבור הצד הזה; AIPlayer בוחר מתוכה.
	std::vector<const Tile*> getSpawnableTiles(const Monster* monster, PlayerSide side) const;

	//bool AI_SpawnMonster(Monster* monster, PlayerSide side);
	void performAction(BoardEntity* entity,const Tile* targetTile);// פונקציית ליבה שמבצעת את הפעולה הפיזית על הלוח (משותפת לאדם ולמחשב)
	//void performAction(Monster* monster, Tile* targetTile);// פונקציית ליבה שמבצעת את הפעולה הפיזית על הלוח (משותפת לאדם ולמחשב)

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
	bool selectEntity(const BoardEntity* entity, PlayerSide side); //למה יש את הפונקציה הזאת בלוח???? מה היא עושה?????
	//bool selectMonster(Monster* monster, PlayerSide side);


	// Highlights every Tile in `tiles` with `color` - the generic operation
	// behind highlightNeighbors/highlightSpawnTiles below, exposed so a
	// caller with its own caller-specific Tile selection (e.g. GameplayState
	// highlighting valid Special-ability targets) can ask Board to paint
	// them instead of calling Tile::setHighlighted directly. Knows nothing
	// about *why* these tiles are being highlighted - same "Board owns Tile
	// painting, never a bystander" principle highlightNeighbors already
	// follows.
	//void highlightTiles(const std::vector<const Tile*>& tiles, const sf::Color& color); //למה הפונקציה הזאת היא ציבורית??????


	// שלב א' של הפירוק: כל ה-tiles שהמפלצת יכולה להגיע/לתקוף אליהם, בלי לצייר
	// שום דבר. לוגיקה טהורה - אין כאן שום קריאה ל-setHighlighted. גם highlightNeighbors
	// (לקליק אנושי) וגם AIPlayer (בעתיד, לצורך ההיוריסטיקה) ישתמשו באותה פונקציה הזו,
	// כדי שה-BFS לא ישוכפל בשני מקומות.
	//
	// Board stays the facade for this (and the two queries below) - the
	// actual BFS lives in BoardPathfinder (see m_pathfinder below); this
	// just forwards. No caller (GameplayState, AIPlayer, Board's own
	// performMove/highlightNeighbors) needs to know that or change anything.
	//std::vector<Tile*> getReachableTiles(Monster* monster) const;
	std::vector<const Tile*> getReachableTiles(const BoardEntity* entity) const;

	// Same reachability query, narrowed to tiles that actually hold
	// something - a pure occupancy fact on top of getReachableTiles, same
	// spirit as the old (now-removed) getOccupiedTiles but range-bound.
	// Decides nothing about ally/enemy validity - that stays entirely with
	// Monster::isValidSpecialTarget, callers still ask that themselves.
	// Shared by highlightValidSpecialTargets (painting) and AIPlayer
	// (deciding which Special target to use), so neither re-derives
	// "which reachable tiles are occupied" on its own.
	std::vector<const Tile*> getReachableOccupiedTiles(const BoardEntity* entity) const;

	// Enemy tiles reachable ONLY because of a monster's extended attack
	// range (Monster::getAttackRange() > getRange()) - i.e. beyond normal
	// move/attack reach but still within the extended reach. NOT movement-legal
	// (never appears in getReachableTiles, so performAction's movement branch
	// already rejects them - see there), purely an additional set for
	// highlighting "this monster can strike here but not stand here."
	// Commented out (not deleted): this served Barzilla's OLD Empowered
	// Attack, which used to extend his own getAttackRange() - now that it's
	// ally-targeted instead (see Barzilla.h), no monster overrides
	// getAttackRange() any more, so this would always be empty for everyone.
	/*std::vector<Tile*> getExtendedAttackOnlyTiles(Monster* monster) const;*/
	//std::vector<const Tile*> getExtendedAttackOnlyTiles(const BoardEntity* entity) const;

	// שלב ב': אותה שאילתה, אבל מחזירה את המסלול המדורג (לפי סדר) מהמפלצת ל-target
	// הספציפי, לא רק "מה אפשר". target חייב להיות tile שכבר יצא מ-getReachableTiles
	// (כלומר תנועה, לא תקיפה) - אחרת מוחזרת רשימה ריקה.
	//std::vector<Tile*> getPathTo(Monster* monster, Tile* target) const;
	std::vector<const Tile*> getPathTo(BoardEntity* entity, Tile* target) const;

	//bool spawnMonsterOnTile(Monster* monster, Tile* targetTile);
	bool spawnEntityOnTile(BoardEntity* entity,const Tile* targetTile);

	// מקור רנדומליות אחד ומשותף לכל הלוח (במקום std::mt19937 מקומי במקום אחד ו-rand() במקום אחר)
	static std::mt19937& rng()
	{
		static std::mt19937 gen(std::random_device{}());
		return gen;
	}
	// Board.h - להוסיף תחת public:
	bool isSpawnPositionValid(const sf::Vector2f& pos) const;

	// The board's own vertical center row - forwards to BoardLayout's own
	// computation (see BoardGenerator.h) rather than Board re-deriving it
	// from m_layout.rows itself. Public so a caller like AIPlayer can ask
	// "where's the middle of the board" as a pure geometry fact, with no
	// need to know Hearts exist there at all (see getExtremeTileInRow below -
	// that's the other half of the same fact: Board::initPlayerHearts places
	// each side's Heart at the extreme tile of exactly this row).
	int getMiddleRow() const { return m_layout.middleRow(); }

	// The extreme (leftmost/rightmost) Tile in `row` - a plain board-shape
	// fact, same spirit as getTileAt. Public (moved up from the private
	// section below) so AIPlayer can use it - together with getMiddleRow()
	// above - to find "the tile at the opponent's back row" as pure
	// geometry, without needing to know a Heart lives there.
	const Tile* getExtremeTileInRow(int row, bool findLeftmost) const;

	// Board-fact lookups reused by anything that needs "what tile is at
	// this coordinate/screen position" - e.g. Blue's knockback (coordinate
	// arithmetic -> Tile) and special-ability target selection (a screen
	// click -> Tile). Board is the Information Expert for tile occupancy;
	// these are the two genuinely-missing general primitives, not
	// duplicates of the range/reachability/occupancy queries above.
	const Tile* getTileAt(int q, int row) const;
	const Tile* getTileAtScreenPosition(const sf::Vector2f& pos) const;


	// Every currently-occupied Tile, board-wide - a plain occupancy fact.
	// Commented out (not deleted): its only caller, highlightValidSpecialTargets,
	// now bounds candidates by range (getReachableTiles) instead of checking
	// the whole board, so this is unused - kept in case a future need for
	// "every occupied tile, unbounded by range" comes up again.
	/*std::vector<const Tile*> getOccupiedTiles() const;*/
	//void highlightNeighbors(const BoardEntity* entity); //למה זה ציבורי??????
	void highlightValidSpecialTargets(const Monster* caster);
	// Pushes `entity` up to `maxTiles` steps in direction (dq, dr), stopping
	// at the first blocked/occupied/off-board tile - purely mechanical
	// (finding tiles, checking passability/occupancy, relocating the
	// entity), same as performMove. `maxTiles` is NOT a Board-owned number -
	// it's the caller's own Special's balance value (see Blue::onSpecialAbility,
	// which passes its own Knockback distance) so Board never hardcodes a
	// specific ability's numbers.
	void applyKnockback(BoardEntity* entity, int dq, int dr, int maxTiles);
private:
	Tile* getMutableTileAt(int q, int row) const;
	void highlightNeighbors(const BoardEntity* entity); //למה זה ציבורי??????

	// Generic "paint these tiles this color" primitive - private (an
	// internal implementation detail Board uses on itself), now actually
	// shared by highlightNeighbors/highlightSpawnTiles/highlightValidSpecialTargets
	// instead of each repeating its own get-mutable-then-setHighlighted loop.
	// Takes const Tile* (what every tile query - getReachableTiles included -
	// already returns) and does the mutable lookup internally via
	// getMutableTileAt, so no caller needs to do that conversion itself.
	// Default color matches Tile::setHighlighted's own default (plain green -
	// "movable"), so a caller that just wants that doesn't need to repeat it.
	void highlightTiles(const std::vector<const Tile*>& tiles, const sf::Color& color = sf::Color(150, 220, 150, 180));
	//std::vector<Tile*> getOccupiedTiles() const;
	// performAction()'s two independent branches, split out so each reads
	// as one responsibility. performAttack coordinates the attack (wires an
	// animation if the attacker supplies one, otherwise resolves through
	// Tile::receiveAttackFrom immediately) - it never computes a damage
	// number or inspects concrete Monster types; that stays entirely below
	// Tile::receiveAttackFrom, inside Monster::attack(). performMove keeps
	// the existing movement coordination (reachability/path/target-tile
	// handling) unchanged.
	void performAttack(BoardEntity* entity, Tile* targetTile);
	//void performMove(Monster* monster, Tile* targetTile);
	/*void performAttack(BoardEntity* entity, Tile* targetTile);
	void performMove(Monster* monster, Tile* targetTile);*/
	void performMove(BoardEntity* entity, Tile* targetTile);

	//void setHighlight(const sf::Vector2f& pos, int range);
	//sf::Vector2f tileToScreen(int q, int row) const;
	//void highlightNeighbors(int q, int row, int range);
	//void highlightNeighbors(Monster* monster); // שינוי חתימה
	//void highlightNeighbors(BoardEntity* entity);
	//Tile* getLeftmostTileInRow(int row) const;
	//Tile* getRightmostTileInRow(int row) const;
	// getExtremeTileInRow moved up to the public section above.
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

	// Both now thin wrappers around BoardGenerator (see BoardGenerator.h):
	// createBoard() builds the plain grid from m_layout at construction
	// time; generateSpecialTiles() swaps in that same layout's special
	// tiles once Hearts exist (called from initPlayerHearts, exactly as
	// before). Board owns *when* generation happens (its own lifecycle);
	// BoardGenerator owns *how* a layout's tiles are actually built - it,
	// not Board, is what now knows LavaTile/Hole/PanicPoint exist.
	void generateSpecialTiles(Heart* p1Heart, Heart* p2Heart);
	void createBoard();

	// This board's own layout recipe (shape + special-tile plan) - kept so
	// generateSpecialTiles() can still consult it later, once Hearts exist.
	// See BoardGenerator.h for what a future second map/stage would pass
	// here instead.
	BoardLayout m_layout;

	// ��� public:
	//std::vector <Monster*> m_monsters;
	std::map<std::pair<int, int>, std::unique_ptr<Tile>> m_grid;

	// Owns the reachability/pathfinding BFS - see BoardPathfinder.h. Reads
	// m_grid by reference, so it must be declared (and therefore
	// constructed) after it - binds once, at Board's own construction, and
	// stays valid for Board's whole lifetime since m_grid's identity (not
	// its contents) never changes afterward.
	BoardPathfinder m_pathfinder;

	// Board layout constants
	static constexpr float START_X = 320.f; //לעשות עוד ללוח ?????נגיד הקצוות
	static constexpr float START_Y = 30.f;
};