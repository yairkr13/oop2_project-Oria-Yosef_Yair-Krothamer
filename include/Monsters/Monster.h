#pragma once
#include "Constants.h"
#include "BoardEntity.h"
#include "SpriteSheet.h"
#include "SpriteAnimator.h"
#include <string>
#include <deque>
#include <memory>
#include <functional>
class Board; // Forward declaration - only ever used by reference in Special Ability hooks below

// Base class for every concrete monster (Blue, Barzilla, Mozzy, Muffintop,
// Henrietta). Owns movement, attacking, the Special Ability framework,
// action/cooldown bookkeeping, and the sprite-sheet animation state machine
// shared by all of them.
class Monster :public BoardEntity
{
public:
    Monster(PlayerSide side, int health, int attackPower, int range, int baseCooldown, int q, int row, sf::Color color, const std::string& textureKey, bool m_flying = false);
    // Declared here, defined "= default" out-of-line in Monster.cpp: m_attackAnimation
    // below is a unique_ptr<AttackAnimation>, and AttackAnimation is only
    // forward-declared in this header (via BoardEntity.h) - same reason
    // BoardEntity::createAttackAnimation's body had to move out-of-line.
    virtual ~Monster();
    void draw(sf::RenderWindow& window, PlayerSide CurrentTurnSide) const override;
    void attack(BoardEntity* target);
    // isAlive() is checked explicitly (not just implied): a dead monster
    // stays linked to its Tile for as long as its death animation is
    // playing (see isDying()/isReadyForRemoval() below), so without this
    // check it would otherwise still satisfy "not an enemy, has actions
    // left" and be selectable/movable/attackable-with while visibly dying.
    bool canBeSelectedBy(PlayerSide side) const override;
    virtual bool canBeTargetedBySpecial() const override;

    bool isOnBoard() const;

    int getRange() const override;

    int getAttackDamage() const;

    int getActionsLeft() const;
    void resetActions();
    PlayerSide getSide() const override;

    void moveAlongPath(int finalQ, int finalRow, const std::vector<sf::Vector2f>& pathScreenPositions) override;

    void update(float dt) override;
    virtual bool canFly() const override;
    bool isMoving() const override;

    bool isAttacking() const override;
    void playAttackAnimation(std::unique_ptr<AttackAnimation> animation) override;

    bool isUsingSpecialAnimation() const override;
    void playSpecialAbilityAnimation(std::unique_ptr<AttackAnimation> animation) override;

    int getSpecialCooldown() const;
    bool isSpecialReady() const;

    bool canUseSpecialAbilityNow() const;

    void applyFreeze() override;

    void applyEmpoweredAttack(float multiplier) override;

    // Whether this monster's Special requires a target before it can
    // commit. Pure virtual: every concrete monster states this explicitly.
    virtual bool specialAbilityNeedsTarget() const = 0;

    virtual bool specialAbilityCommitsOnSelect() const;

    virtual void cancelSpecialAbility();

    virtual std::string getSpecialAbilityDescription() const = 0;
    virtual bool isValidSpecialTarget(const BoardEntity& candidate) const;

    virtual float scoreAsSpecialTarget(const BoardEntity& candidate) const;

    // Tile-highlight color for this monster's valid Special targets. Pure
    // virtual: every concrete monster states its own, even where it just
    // matches the base default.
    virtual sf::Color getSpecialTargetHighlightColor() const = 0;

    virtual bool useSpecialAbility(const Board& board, BoardEntity* target = nullptr);

    bool isDying() const override;

    bool isReadyForRemoval() const override;

    bool canMove() const override;
protected:
    virtual void onSpecialAbility(const Board& board, BoardEntity* target) = 0;

    void setWalkAnimation(const std::string& walkTextureKey, int columns, int rows, float frameDuration);

    void setAttackSpriteAnimation(const std::string& attackTextureKey, int columns, int rows, float frameDuration);

    void setIdleSpriteAnimation(const std::string& idleTextureKey, int columns, int rows, float frameDuration);

    void setDieSpriteAnimation(const std::string& dieTextureKey, int columns, int rows, float frameDuration);

    void setStandardSpriteAnimations(const std::string& texturePrefix, const std::string& walkTextureKey,
        float attackFrameDuration);

    const PlayerSide m_side; // reached directly by some subclasses (e.g. Henrietta's isValidSpecialTarget)

private:
    // None of the 5 concrete monster .cpp files touch any of these
    // directly - each only ever goes through Monster's own public
    // accessors/methods. Only BoardEntity's own m_q/m_row/m_screenPos
    // (inherited, still protected there) and Monster's own m_side above are
    // genuinely reached directly by a subclass.
    int m_attackDamage;
    int m_range;
    sf::Color m_color;
    bool m_flying;
    bool m_frozen = false;

    float m_attackMultiplier = 1.f; // 1.f = no effect; set by an ally's Empowered Attack, consumed on next attack
    std::string m_textureKey;
    std::deque<sf::Vector2f> m_pathQueue; // screen positions this monster is walking through, one per step
    bool m_isMoving = false;

    std::unique_ptr<AttackAnimation> m_attackAnimation; // this monster's own in-flight attack animation, if any

    std::unique_ptr<AttackAnimation> m_specialAnimation; // incoming Special-effect animation (e.g. a heal glow)

    float m_speed = 180.f; // board-travel speed, pixels/second, shared by every monster
    bool m_hasTexture = true;
    float m_baseScale = 1.0f;
    mutable sf::Sprite m_sprite;

    int m_baseCooldown;
    int m_specialCooldown;
    int m_actionsLeft = 2;

    void useAction();
    void drawActionsLeft(sf::RenderWindow& window) const;

    std::unique_ptr<SpriteSheet> configureSpriteSheet(const std::string& textureKey, int columns, int rows, float frameDuration, bool looping = true) const;

    // Ids for Monster's four registered animation states (see m_animator
    // below) - meaningful only inside this class.
    enum class AnimState : int { Idle, Walk, Attack, Die };

    void addAnimationState(AnimState id, std::unique_ptr<SpriteSheet> sheet,
        std::function<bool()> isActive, int priority);

    SpriteAnimator m_animator; // owns all four sprite sheets and decides which is active each frame
};
