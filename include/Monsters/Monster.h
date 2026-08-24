#pragma once
#include "Constants.h"
#include "BoardEntity.h"
#include <string>
#include <deque>
class Board; // Forward declaration - only ever used by reference in Special Ability hooks below
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
    bool canBeSelectedBy(PlayerSide side) const override {
        return !isEnemyOf(side) && m_actionsLeft > 0;
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
    virtual bool isValidSpecialTarget(BoardEntity& candidate) const
    {
        return candidate.asMonster() != nullptr && candidate.isEnemyOf(m_side);
    }

    // The Tile-highlight color for this monster's valid Special targets,
    // shown while target-selection is pending (see GameplayState). Only
    // exercised for monsters where specialAbilityNeedsTarget() is true - a
    // presentation detail belonging to the ability itself, not to Board or
    // GameplayState, which is why it's exposed here rather than decided
    // externally by checking which monster this is.
    virtual sf::Color getSpecialTargetHighlightColor() const { return sf::Color(255, 255, 255, 180); }

    virtual bool useSpecialAbility(Board& board, BoardEntity* target = nullptr);
protected:
    //virtual void onAttackHook(BoardEntity* target) {}
    virtual void onSpecialAbility(Board& board, BoardEntity* target) {}

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

    float m_speed = 300.f;
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
     // �� ����� ������ �� 2 ������
};