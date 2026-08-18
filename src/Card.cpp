//#include "Card.h"
//#include "AssetsManager.h"
//#include "Constants.h"
//
//Card::Card(sf::IntRect rect, std::shared_ptr<Monster> monster, Button::Func onClick)
//    : m_monster(monster),
//    m_position(static_cast<float>(rect.left), static_cast<float>(rect.top)),
//    m_button(rect, AssetsManager::getInstance().getTexture(monster->getCardTextureKey()), onClick)
//{
//}
//
//bool Card::canInteract() const
//{
//    // הקלף זמין ל-Hover ולחיצה רק אם הוא עדיין לא בלוח ואין Cooldown
//    return m_monster && !m_monster->isOnBoard() && (m_monster->getSpecialCooldown() == 0);
//}
//
//void Card::handleEvent(const sf::Event& event)
//{
//    // 💡 ניטרול ה-Hover והלחיצה כשהקלף בלוח או ב-CD:
//    if (!canInteract())
//    {
//        m_button.setHovered(false); // מבטל Hover פעיל במידה והיה
//        return;                     // מתעלם מאירוע העכבר
//    }
//
//    m_button.handleEvent(event);
//}
//
//void Card::update(bool enoughKeys, bool isSelected)
//{
//    m_enoughKeys = enoughKeys;
//    m_isSelected = isSelected;
//}
//
//void Card::draw(sf::RenderWindow& window) const
//{
//    if (!m_monster) return;
//
//    const sf::Font& font = AssetsManager::getInstance().getFont("Lilita");
//    sf::Vector2f position = m_position;
//
//    // 1. ציור מסגרת בחירה (אם הקלף נבחר)
//    if (m_isSelected)
//    {
//        sf::RectangleShape selectionBorder({ Config::CARD_WIDTH + 6.f, Config::CARD_HEIGHT + 6.f });
//        selectionBorder.setPosition({ position.x - 3.f, position.y - 3.f });
//        selectionBorder.setFillColor(sf::Color::Transparent);
//        selectionBorder.setOutlineThickness(3.f);
//        selectionBorder.setOutlineColor(sf::Color(255, 220, 50));
//        window.draw(selectionBorder);
//    }
//
//    // 2. ציור ה-Sprite של הקלף דרך הכפתור הפנימי
//    m_button.draw(window);
//
//    // 3. ציור ה-Cost אם המפלצת עדיין לא בלוח
//    if (!m_monster->isOnBoard())
//    {
//        sf::Text costText(font);
//        costText.setString(std::to_string(m_monster->getCost()));
//        costText.setCharacterSize(22);
//        costText.setFillColor(m_enoughKeys ? sf::Color(255, 100, 100) : sf::Color::White);
//        costText.setOutlineColor(sf::Color::Black);
//        costText.setOutlineThickness(2.f);
//        costText.setPosition({ position.x + 10.f, position.y + 5.f });
//
//        window.draw(costText);
//        return; // מסיים כאן לקלף שטרם הונח
//    }
//
//    // 4. ציור Cooldown / IN PLAY אם המפלצת כבר על הלוח (בדיוק לפי הקוד שלך)
//    sf::Text statusText(font);
//    if (m_monster->getSpecialCooldown() > 0)
//    {
//        statusText.setString("CD: " + std::to_string(m_monster->getSpecialCooldown()));
//        statusText.setFillColor(sf::Color::Yellow);
//    }
//    else
//    {
//        statusText.setString("IN PLAY");
//        statusText.setFillColor(sf::Color(200, 200, 200));
//    }
//
//    statusText.setCharacterSize(16);
//    statusText.setOutlineColor(sf::Color::Black);
//    statusText.setOutlineThickness(2.f);
//
//    sf::FloatRect bounds = statusText.getLocalBounds();
//    statusText.setPosition({
//        position.x + (Config::CARD_WIDTH - bounds.size.x) / 2.f,
//        position.y + (Config::CARD_HEIGHT - bounds.size.y) / 2.f
//        });
//
//    window.draw(statusText);
//}