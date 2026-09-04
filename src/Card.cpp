#include "Card.h"
#include "Monsters/MonsterFactory.h"
#include "Monsters/Monster.h"
#include "AssetsManager.h"

Card::Card(std::string monsterId, int cost, std::string textureKey, PlayerSide side)
    : m_monsterId(std::move(monsterId)), m_cost(cost), m_textureKey(std::move(textureKey)), m_side(side),m_linkedMonster(nullptr)
{
}

//למה פה ולא בmonster factory?
std::unique_ptr<Monster> Card::spawnMonster()
{
    std::unique_ptr<Monster> monster = MonsterFactory::create(m_monsterId, m_side);
    m_linkedMonster = monster.get();
    return monster;
}

void Card::draw(sf::RenderWindow& window, sf::Vector2f position, bool isSelected, bool enoughKeys) const
{
    const sf::Font& font = AssetsManager::getInstance().getFont("Lilita");

    if (isSelected)
    {
		drawBoarder(window, position, sf::Color(255,220,50));
    }

	//calculate scale and position for the card sprite
    const sf::Texture& tex = AssetsManager::getInstance().getTexture(getCardTextureKey());
    sf::Sprite sprite(tex);

    float scaleX = Config::CARD_WIDTH / static_cast<float>(tex.getSize().x);
    float scaleY = Config::CARD_HEIGHT / static_cast<float>(tex.getSize().y);

    sf::Vector2f drawPos = position;
    if (isSelected)
    {
        constexpr float enlarge = 1.08f;
        scaleX *= enlarge;
        scaleY *= enlarge;
        drawPos.x -= (Config::CARD_WIDTH * (enlarge - 1.f)) / 2.f;
        drawPos.y -= (Config::CARD_HEIGHT * (enlarge - 1.f)) / 2.f;
    }

    sprite.setPosition(drawPos);
    sprite.setScale({ scaleX, scaleY });

    if (isPlayed())
        sprite.setColor(sf::Color(255, 255, 255, 100));
	if (m_linkedMonster && m_linkedMonster->isSpecialReady())
		sprite.setColor(sf::Color(255, 255, 255));

    window.draw(sprite);

    // 3. ציור הטקסט המתאים (עלות או סטטוס)
    if (!isPlayed())
    {
        drawCostText(window, drawPos, font, enoughKeys);
    }
    else
    {
        drawStatusText(window, drawPos, font);
    }
}

bool Card::isCardClicked(sf::Vector2f mousePos, sf::Vector2f cardPos) const
{
    sf::FloatRect cardRect(cardPos, { Config::CARD_WIDTH, Config::CARD_HEIGHT });
    return cardRect.contains(mousePos);
}

void Card::drawBoarder(sf::RenderWindow& window, sf::Vector2f position, sf::Color color) const
{
    sf::RectangleShape border({ Config::CARD_WIDTH + 6.f, Config::CARD_HEIGHT + 6.f });
    border.setPosition({ position.x - 3.f, position.y - 3.f });
    border.setFillColor(sf::Color::Transparent);
    border.setOutlineThickness(3.f);
    border.setOutlineColor(color);
    window.draw(border);
}

void Card::drawCostText(sf::RenderWindow& window, sf::Vector2f drawPos, const sf::Font& font, bool enoughKeys) const
{
    sf::Text costText(font);
    costText.setString(std::to_string(m_cost));
    costText.setCharacterSize(22);
    costText.setFillColor(enoughKeys ? sf::Color::White : sf::Color(255, 100, 100));
    costText.setOutlineColor(sf::Color::Black);
    costText.setOutlineThickness(2.f);
    costText.setPosition({ drawPos.x + 10.f, drawPos.y + 5.f });
    window.draw(costText);
}

void Card::drawStatusText(sf::RenderWindow& window, sf::Vector2f drawPos, const sf::Font& font) const
{
    sf::Text statusText(font);
    int cd = m_linkedMonster->getSpecialCooldown();
    if (cd > 0)
    {
        statusText.setString("CD: " + std::to_string(cd));
        statusText.setFillColor(sf::Color::Yellow);
    }
    else
    {
        statusText.setString("READY");
        statusText.setFillColor(sf::Color(120, 255, 120));
    }
    statusText.setCharacterSize(16);
    statusText.setOutlineColor(sf::Color::Black);
    statusText.setOutlineThickness(2.f);

    sf::FloatRect bounds = statusText.getLocalBounds();
    statusText.setPosition({
        drawPos.x + (Config::CARD_WIDTH - bounds.size.x) / 2.f,
        drawPos.y + (Config::CARD_HEIGHT - bounds.size.y) / 2.f
        });
    window.draw(statusText);
}