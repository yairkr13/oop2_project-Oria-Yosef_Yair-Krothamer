#pragma once
#include "GameObject.h"
#include "Constants.h"
#include "BoardEntity.h"
#include <string>
#include <deque>
class Monster :public BoardEntity
{
public:
    Monster(PlayerSide side, const std::string& name, int health, int attackPower, int range/*, int cost,*/ ,int q, int row, sf::Color color, const std::string& textureKey, bool m_flying = false);
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

    int getSpecialCooldown() const { return m_specialCooldown; }
    bool isSpecialReady() const { return m_specialCooldown <= 0; }
    virtual bool useSpecialAbility(BoardEntity* target = nullptr);
protected:
    //virtual void onAttackHook(BoardEntity* target) {}
    virtual void onSpecialAbility(BoardEntity* target) {}

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
    std::string m_textureKey;
    //sf::Vector2f m_targetPos;//private od protected??????????????????????????????
    std::deque<sf::Vector2f> m_pathQueue; // ���: ��� ������ ������, ���� m_targetPos ������
    bool m_isMoving = false;

    // This monster's own in-flight attack animation (see createAttackAnimation/
    // playAttackAnimation) - owned, updated and drawn here, the same
    // ownership model as m_pathQueue/m_isMoving above for movement. Null
    // whenever no attack animation is playing.
    std::unique_ptr<AttackAnimation> m_attackAnimation;

    float m_speed = 300.f;
    bool m_hasTexture = true;
    float m_baseScale = 1.0f;
    mutable sf::Sprite m_sprite;
    int m_specialCooldown = 5; // ���� ������� �� ������� ������� ���� ����� ���
    int m_actionsLeft = 2;
private:
    void drawActionsLeft(sf::RenderWindow& window) const;
    void useAction() { if (m_actionsLeft > 0) m_actionsLeft--; }
    //void drawHealthBar(sf::RenderWindow& window) const;
    //std::string getCardTextureKey() const { return m_textureKey + "_card"; }
     // �� ����� ������ �� 2 ������
};