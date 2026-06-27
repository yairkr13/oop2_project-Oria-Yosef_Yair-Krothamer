#include "Monsters/Monster.h"
#include "Constants.h"
#include "TextureManager.h"

Monster::Monster(const std::string& name, int health, int attackPower, int range, int cost, int q, int row, sf::Color color, const std::string& textureKey,bool flying)
    : BoardEntity(q, row, {}, health),
    m_name(name), m_attackDamage(attackPower),
    m_range(range), m_cost(cost), m_color(color), m_textureKey(textureKey), m_flying(flying),
    m_sprite(TextureManager::getInstance().get<sf::Texture>(m_textureKey))
{
    try
    {
        const sf::Texture& texture = m_sprite.getTexture();
        //m_sprite(texture);

        // הגדרת נקודת המרכז
        m_sprite.setOrigin({ texture.getSize().x / 2.f, texture.getSize().y / 2.f});

        // חישוב ושמירת קנה המידה (Scale)
        float maxTextureDim = std::max(static_cast<float>(texture.getSize().x), static_cast<float>(texture.getSize().y));
        m_baseScale = Config::MONSTER_BOARD_SIZE / maxTextureDim;

        //m_sprite.setScale({ m_baseScale, m_baseScale });
        m_hasTexture = true;
    }
    catch (...)
    {
        // אם הטקסטורה חסרה, נדליק דגל שיגיד ל-draw לצייר מעגל צבעוני
        m_hasTexture = false;
    }
}

void Monster::draw(sf::RenderWindow& window, PlayerSide currentTurnSide) const
{
    if (m_q == -1 && m_row == -1) return;

    if(m_hasTexture)
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
    int TILE_RADIUS = 48.f;
    sf::Text actionText(TextureManager::getInstance().get<sf::Font>("Lilita"));
    actionText.setString(std::to_string(m_actionsLeft));
    actionText.setCharacterSize(14); // גודל קטן שלא יפריע
    actionText.setFillColor(sf::Color::Yellow); // צבע בולט

    // מיקום: קצת מעל המפלצת, או ליד מד החיים
    // m_screenPosition הוא המרכז של המשבצת שהמפלצת נמצאת עליה
    actionText.setPosition({ m_screenPos.x - 15.f, m_screenPos.y + TILE_RADIUS - 20.f });

    window.draw(actionText);
}
//
//void Monster::drawHealthBar(sf::RenderWindow& window) const
//{
//    if (!isAlive()) return;
//    /*if (m_side == currentPlayer)
//        return;*/
//    // 1. הגדרת מידות למד החיים (למשל, 80% מרוחב משבצת הלוח)
//    float barWidth = Config::MONSTER_BOARD_SIZE * 0.8f;
//    float barHeight = 6.f;
//
//    // 2. חישוב מיקום מעל ראש המפלצת (בהנחה ש-m_screenPos הוא מרכז הדמות)
//    float x = m_screenPos.x - (barWidth / 2.f);
//    float y = m_screenPos.y - (Config::MONSTER_BOARD_SIZE / 2.f) - 12.f; // 12 פיקסלים מעל הדמות
//
//    // 3. ציור רקע מד החיים (אדום כהה או אפור)
//    sf::RectangleShape bgBar({ barWidth, barHeight });
//    bgBar.setPosition({ x, y });
//    bgBar.setFillColor(sf::Color(80, 20, 20)); // אדום עמוק
//
//    // 4. חישוב יחס החיים שנותרו וציור המד הירוק
//    float healthRatio = static_cast<float>(m_health) / m_maxHealth;
//    if (healthRatio < 0.f) healthRatio = 0.f;
//
//    sf::RectangleShape fgBar({ barWidth * healthRatio, barHeight });
//    fgBar.setPosition({ x, y });
//    fgBar.setFillColor(sf::Color(50, 220, 50)); // ירוק בהיר
//
//    // 5. רישום על המסך
//    window.draw(bgBar);
//    window.draw(fgBar);
//}
//
//bool Monster::contains(sf::Vector2f point, sf::Vector2f screenPos) const
//{
//    float dx = point.x - screenPos.x;
//    float dy = point.y - screenPos.y;
//    return (dx * dx + dy * dy) <= (32.f * 32.f);
//}



void Monster::walkTo(const sf::Vector2f& targetScreenPos)
{
    m_targetPos = targetScreenPos;
    m_isMoving = true;
}
void Monster::moveTo(int q, int row, const sf::Vector2f& screenPos)
{
    if (m_actionsLeft <= 0) return;

    // קריאה לפונקציה הקיימת שמזיזה אותה גרפית ומעדכנת קואורדינטות
    spawnOnBoard(q, row, screenPos);

    useAction(); // המפלצת מורידה לעצמה פעולה!
}

void Monster::update(float dt)
{
    if (!m_isMoving) return;

    float dx = m_targetPos.x - m_screenPos.x;
    float dy = m_targetPos.y - m_screenPos.y;
    float distance = std::sqrt(dx * dx + dy * dy);

    if (distance < 5.f)
    {
        m_screenPos = m_targetPos;
        m_isMoving = false;
    }
    else
    {
        m_screenPos.x += (dx / distance) * m_speed * dt;
        m_screenPos.y += (dy / distance) * m_speed * dt;
    }
}
//
////
//void Monster::spawnOnBoard(int q, int row, const sf::Vector2f& screenPos) {
//    m_q = q;
//    m_row = row;
//    m_screenPos = screenPos;
//}

bool Monster::isOnBoard() const {
    return m_q != -1 && m_row != -1;
}
//bool isClicked(sf::Vector2f mousePos) const {
//    if (isOnBoard()) {
//        // בדיקת לחיצה על הלוח (עיגול או ה-Sprite הנוכחי)
//        float dx = mousePos.x - m_screenPos.x;
//        float dy = mousePos.y - m_screenPos.y;
//        return (dx * dx + dy * dy) <= (32.f * 32.f);
//    }
//    else {
//        // אם היא ביד, ה-Sprite מכיל את המיקום העדכני שלו מה-draw
//        return m_sprite.getGlobalBounds().contains(mousePos);
//    }
//}
void Monster::drawAsCard(sf::RenderWindow& window, sf::Vector2f position, bool isSelected, bool enoughKeys) const
{
    const sf::Font& font = TextureManager::getInstance().get<sf::Font>("Lilita");

    // ציור מסגרת זהב אם נבחר
    if (isSelected)
    {
        sf::RectangleShape selectionBorder({ Config::CARD_WIDTH + 6.f, Config::CARD_HEIGHT + 6.f });
        selectionBorder.setPosition({ position.x - 3.f, position.y - 3.f });
        selectionBorder.setFillColor(sf::Color::Transparent);
        selectionBorder.setOutlineThickness(3.f);
        selectionBorder.setOutlineColor(sf::Color(255, 220, 50));
        window.draw(selectionBorder);
    }

    // טעינת טקסטורת הקלף
    std::string texKey = getCardTextureKey();
    const sf::Texture& monsterTex = TextureManager::getInstance().get<sf::Texture>(texKey);
    //m_sprite.setTexture(monsterTex); // משתמשים ב-m_sprite של המחלקה
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

    // ציור טקסט עלות מפתחות
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

//void Monster::attack(std::shared_ptr<Monster> target) 
void Monster::attack(BoardEntity* target)
{ 
    target->takeDamage(m_attackDamage);
    useAction(); // המפלצת מורידה לעצמה פעולה!
}
