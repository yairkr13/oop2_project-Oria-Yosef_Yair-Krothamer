#pragma once
#include "Constants.h"
#include "BoardEntity.h"
#include "SpriteSheetAnimation.h"
#include <string>
#include <deque>
#include <memory>
class Board; // Forward declaration - only ever used by reference in Special Ability hooks below

// One state-driven sprite-sheet slot: the frame-cycling logic
// (SpriteSheetAnimation), the sheet's own texture, and the per-frame
// origin/scale needed to keep it centered and sized consistently with a
// monster's static sprite. Bundled once here so a second state (attacking)
// reuses exactly the same centering/scaling math a first state (walking)
// already needed, instead of a second hand-copied implementation - see
// Monster::configureSpriteSheet(), the one place that math lives.
//להפוך למחלקה
struct MonsterSpriteSheet
{
    std::unique_ptr<SpriteSheetAnimation> animation;
    const sf::Texture* texture = nullptr;
    sf::Vector2f frameOrigin;
    float baseScale = 1.f;

    bool isConfigured() const { return animation != nullptr; }
};

class Monster :public BoardEntity
{
public:
    Monster(PlayerSide side, const std::string& name, int health, int attackPower, int range, int baseCooldown/*, int cost,*/ ,int q, int row, sf::Color color, const std::string& textureKey, bool m_flying = false);
    // Declared here, defined "= default" out-of-line in Monster.cpp: m_attackAnimation
    // below is a unique_ptr<AttackAnimation>, and AttackAnimation is only
    // forward-declared in this header (via BoardEntity.h) - same reason
    // BoardEntity::createAttackAnimation's body had to move out-of-line.
    virtual ~Monster();
    void draw(sf::RenderWindow& window, PlayerSide currentTurnSide) const override;
    //void drawAsCard(sf::RenderWindow& window, sf::Vector2f position, bool isSelected, bool enoughKeys) const;
    // 
    //void spawnOnBoard(int q, int row, const sf::Vector2f& screenPos);
   /* void takeDamage(int damage) override;
    bool isAlive() const;*/
    // bool contains(sf::Vector2f point, sf::Vector2f screenPos) const;
     //void attack(std::shared_ptr<Monster> target); 
    void attack(BoardEntity* target);
    //void setSelected(bool selected) { m_selected = selected; }
    //bool isSelected() const override { return m_selected; }
    //virtual bool isSelectable() const override { return true; } // ����� ���� �����!
    // isAlive() is checked explicitly (not just implied): a dead monster
    // stays linked to its Tile for as long as its death animation is
    // playing (see isDying()/isReadyForRemoval() below), so without this
    // check it would otherwise still satisfy "not an enemy, has actions
    // left" and be selectable/movable/attackable-with while visibly dying.
    bool canBeSelectedBy(PlayerSide side) const override {
        return isAlive() && !isEnemyOf(side) && m_actionsLeft > 0;
    }
    virtual Monster* asMonster() override { return this; }

    //virtual EntityType getType() const override { return EntityType::Monster; }
    bool isOnBoard() const;
	bool isClicked(sf::Vector2f mousePos) const; //why we dont use this function in the game??????
    /*bool isCardClicked(sf::Vector2f mousePos, sf::Vector2f cardPosition) const;*/

    //int getCost() const { return m_cost; }
    int getRange() const override { return m_range; }

    // How far this monster can ATTACK - separate from getRange() (which
    // Board's reachability BFS also uses for movement), so a monster whose
    // Special temporarily extends its reach (see Barzilla's Empowered
    // Attack) can do so for attacking only, without also letting it move
    // farther. Defaults to getRange() - i.e. no difference at all - which
    // is correct for every monster that doesn't override this.
    virtual int getAttackRange() const { return getRange(); }

    int getActionsLeft() const { return m_actionsLeft; }
    void resetActions();
    //const std::string& getName() const { return m_name; }
    //std::string getTextureKey() const { return m_textureKey ; }
    //std::string getCardTextureKey() const { return m_textureKey + "_card" ; }
    //void setScreenPosition(const sf::Vector2f& pos) { m_screenPos = pos; }
    //void setSide(PlayerSide side) { m_side = side; }
    PlayerSide getSide() const override { return m_side; }
    void moveTo(int q, int row, const sf::Vector2f& screenPos);

    // ���: ��� moveTo, ��� ����� ���� ��� ��� ���� �����, ������ ��� �� �����
    // ������ (pathScreenPositions) ����. finalQ/finalRow �������� ������ (�����
    // ��� �-moveTo ������) - �� ����� �� ������ ��� ����.
    void moveAlongPath(int finalQ, int finalRow, const std::vector<sf::Vector2f>& pathScreenPositions);

    void walkTo(const sf::Vector2f& targetScreenPos);//chanfe to animation!!!!!!!!!!!!!!!
    void update(float dt) override;
    virtual bool canFly() const override { return m_flying; } // ������ ���� ������ �� �������
    bool isMoving() const override { return m_isMoving; }

    // True exactly while m_attackAnimation is set - no separate bool flag
    // needed (unlike m_isMoving/m_pathQueue): the pointer's presence already
    // is the state.
    bool isAttacking() const override { return m_attackAnimation != nullptr; }
    void playAttackAnimation(std::unique_ptr<AttackAnimation> animation) override;

    // Same ownership model, separate slot - see BoardEntity::isUsingSpecialAnimation
    // for why this isn't just reusing m_attackAnimation.
    bool isUsingSpecialAnimation() const override { return m_specialAnimation != nullptr; }
    void playSpecialAbilityAnimation(std::unique_ptr<AttackAnimation> animation) override;

    int getSpecialCooldown() const { return m_specialCooldown; }
    bool isSpecialReady() const { return m_specialCooldown <= 0; }

    // True while frozen (see Mozzy). Kept as its own explicit, named state -
    // deliberately NOT inferred from m_actionsLeft == 0, since "frozen" and
    // "already spent my actions this turn" are different domain concepts
    // that happen to look the same in that one field; collapsing them would
    // make it impossible to ever query "is this monster frozen specifically"
    // (for a UI indicator, or a future ability that cares) later.
    bool isFrozen() const { return m_frozen; }

    // Applied by an external Freeze-style ability. Fits the existing turn
    // system rather than a new timer: it zeroes this monster's actions
    // immediately (so it cannot act during its own owner's very next turn,
    // which is the only turn that hasn't had resetActions() called on it
    // yet since the freeze), and resetActions() - already called exactly
    // once per owner-turn-end - clears m_frozen the next time it runs,
    // which is precisely when that one blocked turn has concluded.
    void applyFreeze();

    // Whether this monster's Special requires the player to select a
    // target before it can commit (see GameplayState). False (the default)
    // covers self-only Specials.
    virtual bool specialAbilityNeedsTarget() const { return false; }

    // Whether useSpecialAbility() commits (consumes the action, resets the
    // cooldown) at the moment it's called - true (the default) is correct
    // for every Special where being selected/targeted IS using it. A
    // monster whose Special is instead armed now and actually used at some
    // later, separate event overrides this to false - see Barzilla, whose
    // Card click only arms its next attack; the attack itself is what
    // commits (see Barzilla::attack()).
    virtual bool specialAbilityCommitsOnSelect() const { return true; }

    // Un-arms a Special that was armed (see specialAbilityCommitsOnSelect())
    // but never reached its own commit event - e.g. the player clicked the
    // Card again, picked a different Card, or ended the turn without
    // attacking. Default: no-op, correct for every Special that commits on
    // select (nothing was ever armed, so there is nothing to undo).
    virtual void cancelSpecialAbility() {}

    // Whether `candidate` is a legal target for this monster's Special,
    // once one is required. Default: any on-board enemy Monster - covers
    // Mozzy's Freeze and Blue's Knockback without either needing to
    // override this. Ally-targeted Specials (Muffintop's Heal, Henrietta's
    // Protection) override this to require the same side instead.
    // GameplayState calls only this - it never hardcodes "ally" or "enemy"
    // for a specific monster.
    // candidate.isAlive() is required for the same reason canBeSelectedBy()
    // above requires it: a dying candidate is still Tile-linked (see
    // isDying()/isReadyForRemoval()) and must not be targetable while its
    // death animation plays. Henrietta's and Muffintop's ally-targeted
    // overrides need the same check - see their own isValidSpecialTarget.
    virtual bool isValidSpecialTarget(BoardEntity& candidate) const
    {
        return candidate.isAlive() && candidate.asMonster() != nullptr && candidate.isEnemyOf(m_side);
    }

    // The Tile-highlight color for this monster's valid Special targets,
    // shown while target-selection is pending (see GameplayState). Only
    // exercised for monsters where specialAbilityNeedsTarget() is true - a
    // presentation detail belonging to the ability itself, not to Board or
    // GameplayState, which is why it's exposed here rather than decided
    // externally by checking which monster this is.
    virtual sf::Color getSpecialTargetHighlightColor() const { return sf::Color(255, 255, 255, 180); }

    virtual bool useSpecialAbility(Board& board, BoardEntity* target = nullptr);

    // True while dead but still playing a one-shot Die sheet (see
    // setDieSpriteAnimation below) - false once that animation finishes, or
    // immediately/always false if no Die sheet was ever configured. Feeds
    // BoardEntity::isAnimating() (unchanged, ORs this in automatically), so
    // Board/AIPlayer/GameplayState already wait for this exactly like they
    // wait for movement or an attack animation, with no changes of their
    // own needed.
    bool isDying() const override;

    // Overrides BoardEntity's "ready the instant it's dead" default: a
    // monster with a configured Die sheet must also wait for that
    // animation to finish (see isDying() above) before Board clears it from
    // its Tile - a monster with none configured falls straight through to
    // the base behavior, unchanged.
    bool isReadyForRemoval() const override;
protected:
    //virtual void onAttackHook(BoardEntity* target) {}
    virtual void onSpecialAbility(Board& board, BoardEntity* target) {}

    // Opt-in: gives this monster a looping sprite-sheet animation, shown
    // only while isMoving() is true (see update()/draw()) - swapped back to
    // the normal static sprite the instant movement stops. `columns`/`rows`
    // describe the sheet's uniform grid (frame size is derived from the
    // real loaded texture, never hardcoded); `frameDuration` is how long
    // each frame is shown, in seconds - the one knob for animation speed.
    void setWalkAnimation(const std::string& walkTextureKey, int columns, int rows, float frameDuration);

    // Same idea, tied to isAttacking() instead - i.e. shown only while this
    // monster's own m_attackAnimation (the projectile/VFX built by
    // createAttackAnimation) is in flight, swapped back to the static
    // sprite the instant it resolves. Not called by any monster's
    // constructor yet except Muffintop's; every other monster simply never
    // calls this (or setWalkAnimation), so both sheets stay unconfigured
    // and their update()/draw() behave exactly as before, unchanged.
    void setAttackSpriteAnimation(const std::string& attackTextureKey, int columns, int rows, float frameDuration);

    // Same idea again, this time the lowest-priority of the three - shown
    // only while neither the walk sheet nor the attack sheet applies (see
    // draw()'s activeSheet selection: attack beats walk beats idle), i.e.
    // whenever this monster is on the board and not currently moving or
    // attacking. A monster that hasn't called this keeps falling back to
    // its static sprite while idle, exactly as before Idle existed at all -
    // so adding Idle for one monster never affects any other.
    void setIdleSpriteAnimation(const std::string& idleTextureKey, int columns, int rows, float frameDuration);

    // Highest-priority of the four (see draw()'s activeSheet selection: die
    // beats attack beats walk beats idle) and the only non-looping one -
    // built with looping=false (see configureSpriteSheet/SpriteSheetAnimation)
    // so it plays exactly once and holds on its last frame instead of
    // restarting. Driven by isDying() (true from the instant HP reaches 0
    // until this animation finishes), not by any separate flag - once
    // active it never yields back to walk/attack/idle, since isAlive()
    // never becomes true again. A monster that hasn't called this is
    // removed from the board immediately on death, exactly as every
    // monster was before Die sheets existed (see isReadyForRemoval()).
    void setDieSpriteAnimation(const std::string& dieTextureKey, int columns, int rows, float frameDuration);

    std::string m_name;
    //int m_health;
    //int m_maxHealth;
    int m_attackDamage;
    int m_range;
    //int m_cost;
    //int m_q;
    //int m_row;
    //bool m_selected = false;
    sf::Color m_color;
    const PlayerSide m_side;  // �� ���� ������ ���� ������ - ��������� ���� "����" �� ��
    bool m_flying;
    bool m_frozen = false;
    std::string m_textureKey;
    //sf::Vector2f m_targetPos;//private od protected??????????????????????????????
    std::deque<sf::Vector2f> m_pathQueue; // ���: ��� ������ ������, ���� m_targetPos ������
    bool m_isMoving = false;

    // Optional sprite-sheet-driven states (see setWalkAnimation/
    // setAttackSpriteAnimation above) - unconfigured (isConfigured()
    // false) for every monster that hasn't opted into that particular one,
    // which is every monster except Muffintop for now. Each texture
    // pointer is non-owning, into AssetsManager's own texture (same
    // lifetime assumption m_sprite already relies on for m_textureKey).
    MonsterSpriteSheet m_walkSheet;
    MonsterSpriteSheet m_attackSheet;
    MonsterSpriteSheet m_idleSheet;
    MonsterSpriteSheet m_dieSheet;

    // This monster's own in-flight attack animation (see createAttackAnimation/
    // playAttackAnimation) - owned, updated and drawn here, the same
    // ownership model as m_pathQueue/m_isMoving above for movement. Null
    // whenever no attack animation is playing.
    std::unique_ptr<AttackAnimation> m_attackAnimation;

    // This monster's own in-flight Special Ability effect animation (see
    // playSpecialAbilityAnimation above) - e.g. the Heal effect playing on
    // an ally that Muffintop just targeted. Separate from m_attackAnimation
    // above: a monster can be the passive subject of an incoming Special
    // effect without attacking or being attacked, and the two are entirely
    // unrelated events that happen to share the same AttackAnimation
    // interface (update/draw/isFinished/onImpact), not the same slot.
    std::unique_ptr<AttackAnimation> m_specialAnimation;

    // Board travel speed, pixels/second, shared by every monster (see
    // Monster::update()'s movement interpolation) - no subclass overrides
    // this. Lowered from the original 300.f so a tile-to-tile move takes
    // noticeably longer, giving the walking sprite-sheet animation (see
    // setWalkAnimation) enough time on screen to actually read.
    float m_speed = 180.f;
    bool m_hasTexture = true;
    float m_baseScale = 1.0f;
    mutable sf::Sprite m_sprite;

    // Per-monster Special cooldown duration (see BASE_COOLDOWN on each
    // concrete monster) and the current countdown - both instance state,
    // owned here, never by Card or Player. m_specialCooldown starts at the
    // monster's own base value (set in the constructor init list) rather
    // than a fixed literal, so a freshly-spawned monster's Special isn't
    // ready any sooner or later than its own BASE_COOLDOWN says.
    int m_baseCooldown;
    int m_specialCooldown;
    int m_actionsLeft = 2;

    // useAction() is protected (not private): concrete monsters that
    // override attack() (see Barzilla, to apply its empowered-attack
    // multiplier) still need to consume an action exactly like the base
    // Monster::attack() does.
protected:
    void useAction() { if (m_actionsLeft > 0) m_actionsLeft--; }
private:
    void drawActionsLeft(sf::RenderWindow& window) const;
    //void drawHealthBar(sf::RenderWindow& window) const;
    //std::string getCardTextureKey() const { return m_textureKey + "_card"; }

    // Shared by setWalkAnimation/setAttackSpriteAnimation: builds a
    // MonsterSpriteSheet's texture pointer, per-frame origin (the center of
    // ONE frame, not the whole sheet) and base scale (from one frame's real
    // pixel size against Config::MONSTER_BOARD_SIZE - the same reference
    // the static sprite's own m_baseScale already uses) - the one place
    // this math lives, regardless of how many sheet-driven states a
    // monster ends up configuring.
    MonsterSpriteSheet configureSpriteSheet(const std::string& textureKey, int columns, int rows, float frameDuration, bool looping = true) const;

    // Advances `sheet`'s animation while `active` is true, otherwise resets
    // it to frame 0 (so whichever state becomes active next always starts
    // fresh) - a no-op for an unconfigured sheet. Shared by update() for
    // both m_walkSheet/m_isMoving and m_attackSheet/isAttacking().
    static void updateSpriteSheet(MonsterSpriteSheet& sheet, bool active, float dt);
     // �� ����� ������ �� 2 ������
};