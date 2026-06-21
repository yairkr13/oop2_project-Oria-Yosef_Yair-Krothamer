#include "Player.h"
#include "Monsters/Muffintop.h"
#include "Monsters/Blue.h"
#include "Constants.h"
#include "TextureManager.h"

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

void Player::drawHand(sf::RenderWindow& window, bool alignRight, std::shared_ptr<Monster> selectedFromHand) const
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
        bool isSelected = (selectedFromHand && m_monsters[i] == selectedFromHand);

        // Selection visual: golden outline border
        if (isSelected)
        {
            sf::RectangleShape selectionBorder({ Config::CARD_WIDTH + 6.f, Config::CARD_HEIGHT + 6.f });
            selectionBorder.setPosition({ startX - 3.f, Config::CARD_START_Y - 3.f });
            selectionBorder.setFillColor(sf::Color::Transparent);
            selectionBorder.setOutlineThickness(3.f);
            selectionBorder.setOutlineColor(sf::Color(255, 220, 50)); // gold
            window.draw(selectionBorder);
        }

        try
        {
            std::string texKey = m_monsters[i]->getCardTextureKey();
            const sf::Texture& monsterTex = TextureManager::getInstance().getTexture(texKey);
            sf::Sprite monsterSprite(monsterTex);

            float scaleX = Config::CARD_WIDTH / static_cast<float>(monsterTex.getSize().x);
            float scaleY = Config::CARD_HEIGHT / static_cast<float>(monsterTex.getSize().y);

            // Slightly enlarge the selected card
            if (isSelected)
            {
                float enlarge = 1.08f;
                scaleX *= enlarge;
                scaleY *= enlarge;
                // Center the enlarged sprite
                float offsetX = (Config::CARD_WIDTH * (enlarge - 1.f)) / 2.f;
                float offsetY = (Config::CARD_HEIGHT * (enlarge - 1.f)) / 2.f;
                monsterSprite.setPosition({ startX - offsetX, Config::CARD_START_Y - offsetY });
            }
            else
            {
                monsterSprite.setPosition({ startX, Config::CARD_START_Y });
            }

            monsterSprite.setScale({ scaleX, scaleY });
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

void Player::endTurn()
{
	m_keys += 5; // Add 5 keys at the end of the turn
}