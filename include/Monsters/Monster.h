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
    Monster(PlayerSide side, int health, int attackPower, int range, int baseCooldown/*, int cost,*/, int q, int row, sf::Color color, const std::string& textureKey, bool m_flying = false);
    virtual ~Monster();
    void draw(sf::RenderWindow& window, PlayerSide CurrentTurnSide) const override;
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
    bool canBeSelectedBy(PlayerSide side) const override;
    //virtual Monster* asMonster() override { return this; }
    virtual bool canBeTargetedBySpecial() const override;

    //virtual EntityType getType() const override { return EntityType::Monster; }
    bool isOnBoard() const;
    /*bool isCardClicked(sf::Vector2f mousePos, sf::Vector2f cardPosition) const;*/

    //int getCost() const { return m_cost; }
    int getRange() const override;

    int getAttackDamage() const;

    // Unused: would let a Special extend attack range without also
    // extending movement range. No monster currently needs this.
    //virtual int getAttackRange() const { return getRange(); }

    int getActionsLeft() const;
    void resetActions();
    //std::string getTextureKey() const { return m_textureKey ; }
    //std::string getCardTextureKey() const { return m_textureKey + "_card" ; }
    //void setScreenPosition(const sf::Vector2f& pos) { m_screenPos = pos; }
    //void setSide(PlayerSide side) { m_side = side; }
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
    //void moveAlongPath(int finalQ, int finalRow, const std::vector<sf::Vector2f>& pathScreenPositions) override;
protected:
    //virtual void onAttackHook(BoardEntity* target) {}
    virtual void onSpecialAbility(const Board& board, BoardEntity* target) = 0;

    void setWalkAnimation(const std::string& walkTextureKey, int columns, int rows, float frameDuration);

    void setAttackSpriteAnimation(const std::string& attackTextureKey, int columns, int rows, float frameDuration);

    void setIdleSpriteAnimation(const std::string& idleTextureKey, int columns, int rows, float frameDuration);

    void setDieSpriteAnimation(const std::string& dieTextureKey, int columns, int rows, float frameDuration);

    void setStandardSpriteAnimations(const std::string& texturePrefix, const std::string& walkTextureKey,
        float attackFrameDuration);

    const PlayerSide m_side; // reached directly by some subclasses (e.g. Henrietta's isValidSpecialTarget)

private:
    int m_attackDamage;
    int m_range;
    //int m_cost;
    //int m_q;
    //int m_row;
    //bool m_selected = false;
    sf::Color m_color;
    bool m_flying;
    bool m_frozen = false;

    float m_attackMultiplier = 1.f; // 1.f = no effect; set by an ally's Empowered Attack, consumed on next attack
    std::string m_textureKey;
    //sf::Vector2f m_targetPos;//private od protected??????????????????????????????
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
    //void drawHealthBar(sf::RenderWindow& window) const;
    //std::string getCardTextureKey() const { return m_textureKey + "_card"; }

    std::unique_ptr<SpriteSheet> configureSpriteSheet(const std::string& textureKey, int columns, int rows, float frameDuration, bool looping = true) const;

    // Ids for Monster's four registered animation states (see m_animator
    // below) - meaningful only inside this class.
    enum class AnimState : int { Idle, Walk, Attack, Die };

    void addAnimationState(AnimState id, std::unique_ptr<SpriteSheet> sheet,
        std::function<bool()> isActive, int priority);

    SpriteAnimator m_animator; // owns all four sprite sheets and decides which is active each frame
};
