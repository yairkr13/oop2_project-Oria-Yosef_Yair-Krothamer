#pragma once
#include "GameObject.h"
#include "Constants.h"
#include "BoardEntity.h"
#include <string>
#include <deque>
class Monster :public BoardEntity
{
public:
    Monster(PlayerSide side, const std::string& name, int health, int attackPower, int range, int cost, int q, int row, sf::Color color, const std::string& textureKey, bool m_flying = false);
    virtual ~Monster() = default;
    void draw(sf::RenderWindow& window, PlayerSide currentTurnSide) const override;
    void drawAsCard(sf::RenderWindow& window, sf::Vector2f position, bool isSelected, bool enoughKeys) const;
    //void spawnOnBoard(int q, int row, const sf::Vector2f& screenPos);
   /* void takeDamage(int damage) override;
    bool isAlive() const;*/
    // bool contains(sf::Vector2f point, sf::Vector2f screenPos) const;
     //void attack(std::shared_ptr<Monster> target); 
    void attack(BoardEntity* target);
    void setSelected(bool selected) { m_selected = selected; }
    bool isSelected() const override { return m_selected; }
    virtual bool isSelectable() const override { return true; } // מפלצת אפשר לבחור!
    //virtual EntityType getType() const override { return EntityType::Monster; }
    bool isOnBoard() const;
    bool isClicked(sf::Vector2f mousePos) const;
    bool isCardClicked(sf::Vector2f mousePos, sf::Vector2f cardPosition) const;

    int getCost() const { return m_cost; }
    int getRange() const { return m_range; }
    int getActionsLeft() const { return m_actionsLeft; }
    void resetActions() { m_actionsLeft = 2; }
    //const std::string& getName() const { return m_name; }
    //std::string getTextureKey() const { return m_textureKey ; }
    //std::string getCardTextureKey() const { return m_textureKey + "_card" ; }
    //void setScreenPosition(const sf::Vector2f& pos) { m_screenPos = pos; }
    //void setSide(PlayerSide side) { m_side = side; }
    PlayerSide getSide() const override { return m_side; }
    void moveTo(int q, int row, const sf::Vector2f& screenPos);

    // חדש: כמו moveTo, אבל במקום לזוז בקו ישר ליעד הסופי, הולכים דרך כל משבצת
    // שבנתיב (pathScreenPositions) ברצף. finalQ/finalRow מתעדכנים מיידית (בדיוק
    // כמו ב-moveTo המקורי) - רק הציור זז בהדרגה דרך התור.
    void moveAlongPath(int finalQ, int finalRow, const std::vector<sf::Vector2f>& pathScreenPositions);

    void walkTo(const sf::Vector2f& targetScreenPos);//chanfe to animation!!!!!!!!!!!!!!!
    void update(float dt) override;
    virtual bool canFly() const { return m_flying; } // כברירת מחדל מפלצות הן קרקעיות
    bool isMoving() const override { return m_isMoving; }
protected:
    std::string m_name;
    //int m_health;
    //int m_maxHealth;
    int m_attackDamage;
    int m_range;
    int m_cost;
    //int m_q;
    //int m_row;
    bool m_selected = false;
    sf::Color m_color;
    const PlayerSide m_side;  // לא ניתן לשינוי לאחר היצירה - הקומפיילר עצמו "אוכף" את זה
    bool m_flying;
    std::string m_textureKey;
    sf::Vector2f m_targetPos;//private od protected??????????????????????????????
    std::deque<sf::Vector2f> m_pathQueue; // חדש: שאר הצעדים במסלול, אחרי m_targetPos הנוכחי
    bool m_isMoving = false;
    float m_speed = 300.f;
    bool m_hasTexture = true;
    float m_baseScale = 1.0f;
    mutable sf::Sprite m_sprite;
private:
    void drawActionsLeft(sf::RenderWindow& window) const;
    void useAction() { if (m_actionsLeft > 0) m_actionsLeft--; }
    //void drawHealthBar(sf::RenderWindow& window) const;
    std::string getCardTextureKey() const { return m_textureKey + "_card"; }
    int m_actionsLeft = 2; // כל מפלצת מתחילה עם 2 פעולות
};