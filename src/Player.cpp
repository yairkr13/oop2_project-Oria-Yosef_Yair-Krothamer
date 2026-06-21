#include "Player.h"
#include "Muffintop.h"
#include "Blue.h"
#include "Constants.h"
#include "TextureManager.h" 
//#include "Blue.h"

Player::Player()
	: /*m_heart(std::make_unique<Heart>()),*/ m_keys(20)
{
	// Initialize cards
	m_monsters.push_back(std::make_shared<Muffintop>());
    m_monsters.push_back(std::make_shared<Blue>());
	//m_monsters.push_back(std::make_unique<Blue>());
}
/*
void Player::handleClick(sf::Vector2f pos)
{
	for (auto& monster : m_monsters)
	{
		if (!monster->isOnBoard() && monster->contains(pos, screenPos in hand))
		{
			m_selected = monster.get();
			return;
		}
	}
}*/

void Player::drawHand(sf::RenderWindow& window, bool alignRight) const
{
    // 1. ציור "השולחן" (הפאנל התחתון)
    sf::RectangleShape bottomPanel({static_cast<float>(Config::WINDOW_WIDTH), Config::BOTTOM_PANEL_HEIGHT});
    bottomPanel.setPosition({ 0.f, Config::BOTTOM_PANEL_Y });
    bottomPanel.setFillColor(sf::Color(40, 40, 40));
    window.draw(bottomPanel);

    // 2. שולפים מראש את הטקסטורה של הקלף הריק (נניח ששמרת אותה בשם "card_bg")
    const sf::Texture* cardBgTex = nullptr;
    try {
        cardBgTex = &TextureManager::getInstance().getTexture("card_bg");
    }
    catch (...) {}

    for (size_t i = 0; i < m_monsters.size(); ++i)
    {
        float startX;
        if (!alignRight)
        {
            startX = 20.f + (i * Config::CARD_SPACING);
        }
        else
        {
            startX = Config::WINDOW_WIDTH - 20.f - Config::CARD_WIDTH - (i * Config::CARD_SPACING);
        }

        // --- ציור רקע הקלף ---
        if (cardBgTex)
        {
            sf::Sprite cardSprite(*cardBgTex);
            cardSprite.setPosition({ startX, Config::CARD_START_Y });

            float scaleX = Config::CARD_WIDTH / cardBgTex->getSize().x;
            float scaleY = Config::CARD_HEIGHT / cardBgTex->getSize().y;
            cardSprite.setScale({ scaleX, scaleY });

            window.draw(cardSprite);
        }
        else
        {
            sf::RectangleShape fallbackRect({ Config::CARD_WIDTH, Config::CARD_HEIGHT });
            fallbackRect.setPosition({ startX, Config::CARD_START_Y });
            fallbackRect.setFillColor(sf::Color(80, 80, 80));
            fallbackRect.setOutlineThickness(2.f);
            fallbackRect.setOutlineColor(sf::Color::Black);
            window.draw(fallbackRect);
        }

        // --- ציור המפלצת בתוך הקלף ---
        try
        {
            std::string texKey = m_monsters[i]->getName();
            const sf::Texture& monsterTex = TextureManager::getInstance().getTexture(texKey);
            sf::Sprite monsterSprite(monsterTex);

            monsterSprite.setOrigin({ monsterTex.getSize().x / 2.f, monsterTex.getSize().y / 2.f });
            monsterSprite.setPosition({ startX + (Config::CARD_WIDTH / 2.f), Config::CARD_START_Y + (Config::CARD_HEIGHT / 2.f) });

            // --- הנוסחה החכמה לסקייל בתוך הקלף ---
            // אנחנו רוצים שהמפלצת תהיה בגודל מקסימלי של 50 פיקסלים בתוך הקלף
            float targetSizeInCard = 50.f;
            float maxTextureDim = std::max(monsterTex.getSize().x, monsterTex.getSize().y);
            float monsterScale = targetSizeInCard / maxTextureDim;

            monsterSprite.setScale({ monsterScale, monsterScale });
            // ---------------------------------------

            window.draw(monsterSprite);
        }
        catch (...) {}
    }
}

std::shared_ptr<Monster> Player::handleHandClick(sf::Vector2f mousePos, bool alignRight) const
{
    if (mousePos.y < Config::BOTTOM_PANEL_Y) return nullptr;

    for (size_t i = 0; i < m_monsters.size(); ++i)
    {
        // אם המפלצת כבר על הלוח, אי אפשר לבחור אותה שוב מהיד
        if (m_monsters[i]->getQ() != -1) continue;

        float startX;
        if (!alignRight)
            startX = 20.f + (i * Config::CARD_SPACING);
        else
            startX = Config::WINDOW_WIDTH - 20.f - Config::CARD_WIDTH - (i * Config::CARD_SPACING); // תוקן ה-1280!

        // יצירת "מלבן וירטואלי" סביב הקלף עם המידות מהקבועים
        sf::FloatRect cardRect({ startX, Config::CARD_START_Y }, { Config::CARD_WIDTH, Config::CARD_HEIGHT });
        if (cardRect.contains(mousePos))
        {
            return m_monsters[i]; // מצאנו איזה קלף נלחץ!
        }
    }
    return nullptr;
}