#include "Monsters/Monster.h"
#include "Constants.h"
#include "AssetsManager.h"

Monster::Monster(PlayerSide side, const std::string& name, int health, int attackPower, int range, int cost, int q, int row, sf::Color color, const std::string& textureKey, bool flying)
    : BoardEntity(q, row, {}, health),
    m_side(side), m_name(name), m_attackDamage(attackPower),
    m_range(range), m_cost(cost), m_color(color), m_textureKey(textureKey), m_flying(flying),
    m_sprite(AssetsManager::getInstance().getTexture(m_textureKey))
{
    try
    {
        const sf::Texture& texture = m_sprite.getTexture();
        //m_sprite(texture);

        // origin at texture center
        m_sprite.setOrigin({ texture.getSize().x / 2.f, texture.getSize().y / 2.f });

        // scale relative to on-board size
        float maxTextureDim = std::max(static_cast<float>(texture.getSize().x), static_cast<float>(texture.getSize().y));
        m_baseScale = Config::MONSTER_BOARD_SIZE / maxTextureDim;

        //m_sprite.setScale({ m_baseScale, m_baseScale });
        m_hasTexture = true;
    }
    catch (...)
    {
        // no texture available, draw fallback shape in draw()
        m_hasTexture = false;
    }
}

void Monster::draw(sf::RenderWindow& window, PlayerSide currentTurnSide) const
{
    if (m_q == -1 && m_row == -1) return;

    if (m_hasTexture)
    {
        m_sprite.setPosition(m_screenPos);

        float currentScaleX = (m_side == PlayerSide::Right) ? -m_baseScale : m_baseScale;
        m_sprite.setScale({ currentScaleX, m_baseScale });

        window.draw(m_sprite);
    }
    else
    {
        sf::CircleShape circle(Config::MONSTER_BOARD_SIZE / 2.f);
        circle.setFillColor(m_color);
        circle.setOrigin({ 16.f, 16.f });
        circle.setPosition(m_screenPos);
        window.draw(circle);
    }
    if (m_side != currentTurnSide)
    {
        drawHealthBar(window);
    }
    else
        drawActionsLeft(window);
}
void Monster::drawActionsLeft(sf::RenderWindow& window) const
{
    sf::Text actionText(AssetsManager::getInstance().getFont("Lilita"));
    actionText.setString(std::to_string(m_actionsLeft));
    actionText.setCharacterSize(14);
    actionText.setFillColor(sf::Color::Yellow);

    actionText.setPosition({ m_screenPos.x - 15.f, m_screenPos.y + Config::TILE_RADIUS - 20.f });

    window.draw(actionText);
}

void Monster::walkTo(const sf::Vector2f& targetScreenPos)
{
    // צעד יחיד = תור עם פריט אחד. אין יותר m_targetPos נפרד - front() של התור
    // *הוא* היעד הנוכחי, מקור אמת יחיד.
    m_pathQueue.clear();
    m_pathQueue.push_back(targetScreenPos);
    m_isMoving = true;
}

void Monster::moveTo(int q, int row, const sf::Vector2f& screenPos)
{
    if (m_actionsLeft <= 0) return;

    // עדכון המיקום הלוגי מיידי, רק הציור זז בהדרגה דרך update()
    m_q = q;
    m_row = row;
    walkTo(screenPos);  // animate visually instead of teleporting

    useAction();
}

void Monster::moveAlongPath(int finalQ, int finalRow, const std::vector<sf::Vector2f>& pathScreenPositions)
{
    if (m_actionsLeft <= 0 || pathScreenPositions.empty()) return;

    // בדיוק כמו ב-moveTo: המיקום הלוגי מתעדכן מיידית ליעד הסופי. השינוי היחיד הוא
    // שהציור לא "יטפס" בקו ישר אליו, אלא יעבור דרך כל צעד ברשימה, אחד אחרי השני.
    m_q = finalQ;
    m_row = finalRow;

    m_pathQueue.assign(pathScreenPositions.begin(), pathScreenPositions.end());
    m_isMoving = true;

    useAction();
}

void Monster::update(float dt)
{
    // אם לא זזים כרגע, אין מה לעדכן
    if (!m_isMoving || m_pathQueue.empty())
    {
        m_isMoving = false;
        return;
    }

    // 1. וקטור הכיוון והמרחק ליעד הנוכחי - front() של התור, לא משתנה נפרד
    const sf::Vector2f& currentTarget = m_pathQueue.front();
    float dx = currentTarget.x - m_screenPos.x;
    float dy = currentTarget.y - m_screenPos.y;
    float distance = std::sqrt(dx * dx + dy * dy);

    // 2. הגענו לצעד הנוכחי? (מספיק קרוב - פחות מ-5 פיקסלים)
    if (distance < 5.0f)
    {
        m_screenPos = currentTarget; // מיישרים בדיוק לצעד הנוכחי
        m_pathQueue.pop_front();     // סיימנו את הצעד הזה

        if (m_pathQueue.empty())
        {
            // זה היה הצעד האחרון - האנימציה נגמרה
            m_isMoving = false;
        }
        // אחרת: m_isMoving נשאר true, ו-front() הבא ישמש כיעד ב-frame הבא
    }
    else
    {
        // 3. עוד לא הגענו - נזוז צעד קטן לכיוון היעד הנוכחי
        float moveX = (dx / distance) * m_speed * dt;
        float moveY = (dy / distance) * m_speed * dt;

        m_screenPos.x += moveX;
        m_screenPos.y += moveY;
    }

    // 4. עדכון גרפי: מעדכנים את ה-sprite כדי שה-SFML ידע איפה לצייר אותו עכשיו
    m_sprite.setPosition(m_screenPos);
}

bool Monster::isOnBoard() const {
    return m_q != -1 && m_row != -1;
}

void Monster::drawAsCard(sf::RenderWindow& window, sf::Vector2f position, bool isSelected, bool enoughKeys) const
{
    const sf::Font& font = AssetsManager::getInstance().getFont("Lilita");

    if (isSelected)
    {
        sf::RectangleShape selectionBorder({ Config::CARD_WIDTH + 6.f, Config::CARD_HEIGHT + 6.f });
        selectionBorder.setPosition({ position.x - 3.f, position.y - 3.f });
        selectionBorder.setFillColor(sf::Color::Transparent);
        selectionBorder.setOutlineThickness(3.f);
        selectionBorder.setOutlineColor(sf::Color(255, 220, 50));
        window.draw(selectionBorder);
    }

    std::string texKey = getCardTextureKey();
    const sf::Texture& monsterTex = AssetsManager::getInstance().getTexture(texKey);
    sf::Sprite monsterSprite(monsterTex);

    float scaleX = Config::CARD_WIDTH / static_cast<float>(monsterTex.getSize().x);
    float scaleY = Config::CARD_HEIGHT / static_cast<float>(monsterTex.getSize().y);

    if (isSelected)
    {
        float enlarge = 1.08f;
        scaleX *= enlarge;
        scaleY *= enlarge;
        float offsetX = (Config::CARD_WIDTH * (enlarge - 1.f)) / 2.f;
        float offsetY = (Config::CARD_HEIGHT * (enlarge - 1.f)) / 2.f;
        position.x -= offsetX;
        position.y -= offsetY;
    }

    monsterSprite.setPosition(position);
    monsterSprite.setScale({ scaleX, scaleY });
    window.draw(monsterSprite);

    sf::Text costText(font);
    costText.setString(std::to_string(m_cost));
    costText.setCharacterSize(22);
    costText.setFillColor(enoughKeys ? sf::Color(255, 100, 100) : sf::Color::White);
    costText.setOutlineColor(sf::Color::Black);
    costText.setOutlineThickness(2.f);
    costText.setPosition({ position.x + 10.f, position.y + 5.f });

    window.draw(costText);
}

bool Monster::isCardClicked(sf::Vector2f mousePos, sf::Vector2f cardPos) const
{
    sf::FloatRect cardRect(cardPos, { Config::CARD_WIDTH, Config::CARD_HEIGHT });
    return cardRect.contains(mousePos);
}

void Monster::attack(BoardEntity* target)
{
    target->takeDamage(m_attackDamage);
    useAction();
}