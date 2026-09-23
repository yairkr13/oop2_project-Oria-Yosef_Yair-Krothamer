#include "Player.h"
#include "Monsters/MonsterFactory.h"
#include "Constants.h"
#include "Tiles/Tile.h"
#include "AssetsManager.h"

Player::Player(PlayerSide side)
	: m_heart(std::make_unique<Heart>(side, -1, -1, sf::Vector2f(0.f, 0.f))), // temporary position until placed on the board
    m_keys(3),m_maxKeys(12), m_side(side)
{
    m_hand = MonsterFactory::createStandardHand(side);
}

void Player::draw(sf::RenderWindow& window, bool alignRight, const Card* selectedFromHand) const
{
    drawHand(window, alignRight, selectedFromHand);
	drawKeys(window, alignRight);
}

void Player::drawKeys(sf::RenderWindow& window, bool alignRight) const
{
    const sf::Font& font = AssetsManager::getInstance().getFont("Lilita");

    sf::Text keysText(font);
    keysText.setString("Keys: " + std::to_string(m_keys) + " / " + std::to_string(m_maxKeys));
    keysText.setCharacterSize(24);
    keysText.setFillColor(sf::Color::White);

    auto bounds = keysText.getLocalBounds();

    // SFML 3: bounds.size.x instead of width.
    float xPos = alignRight ? static_cast<float>(Config::WINDOW_WIDTH) - 20.f - bounds.size.x : 20.f;

    // SFML 3: bounds.size.y / bounds.position.y for exact vertical centering.
    float yPos = 20.f - bounds.position.y;

    keysText.setPosition({ xPos, yPos });

    window.draw(keysText);
}

void Player::drawHand(sf::RenderWindow& window, bool alignRight, const Card* selectedFromHand) const
{
    for (size_t i = 0; i < m_hand.size(); ++i)
    {
        bool isSelected = (selectedFromHand && m_hand[i].get() == selectedFromHand);

        bool enoughKeys = (m_hand[i]->getCost() <= m_keys);

        m_hand[i]->draw(window, getCardPosition(i, alignRight), isSelected, enoughKeys);
    }
}

std::string Player::getCardTooltipAt(const sf::Vector2f& pos) const
{
    Card* card = getCardAtPosition(pos, m_side == PlayerSide::Right);
    if (!card)
        return "";

    const Monster* monster = card->getLinkedMonster(); // read-only - getSpecialAbilityDescription() below is const
    // A card stays linked even after its monster dies (removeDeadMonsters
    // never runs) - no isAlive() check here, so the tooltip keeps showing
    // on a dead monster's card.
    if (!monster || !monster->isAlive())
        return "";

    return monster->getSpecialAbilityDescription();
}

Card* Player::handleHandClick(sf::Vector2f mousePos, bool alignRight) const
{
    if (mousePos.y < BOTTOM_PANEL_TOP_Y) return nullptr;

    Card* card = getCardAtPosition(mousePos, alignRight);
    // A clicked card that's already played is still returned (so it can be
    // selected for a Special), but a not-yet-played card the player can't
    // afford is treated as if nothing was clicked at all.
    if (card && !card->isPlayed() && card->getCost() > m_keys)
        return nullptr;

    return card;
}

Monster* Player::playCard(Card* card)
{
    // Reject a missing card, one already played, or one this player can't
    // currently afford.
    if (!card || card->isPlayed() || card->getCost() > m_keys)
        return nullptr;

    std::unique_ptr<Monster> monster = card->spawnMonster();

    if (!monster) return nullptr;

    reduceKeys(card->getCost());

    Monster* raw = monster.get();
    m_monsters.push_back(std::move(monster));

    return raw;
}

void Player::removeDeadMonsters()
{
    // Card::isGone() is already true the instant its monster dies, so
    // erasing the whole Card directly (no separate unlink step first) is
    // safe here: GameplayState::update() clears its own m_selectedFromHand/
    // m_pendingSpecialCard first (the only other things that could ever
    // reference one of these Cards), specifically so this erase never runs
    // while something else still points at the Card being destroyed.
    std::erase_if(m_hand, [](const std::unique_ptr<Card>& card) {
        return !card || card->isGone();
    });
}

int Player::indexOfCard(const Card* card) const
{
    for (size_t i = 0; i < m_hand.size(); ++i)
    {
        if (m_hand[i].get() == card)
            return static_cast<int>(i);
    }
    return -1;
}

Card* Player::getCardAtPosition(const sf::Vector2f& mousePos, bool alignRight) const
{
    for (size_t i = 0; i < m_hand.size(); ++i)
    {
        // Skip a null slot defensively, and only match a card whose own
        // on-screen rect actually contains this click.
        if (m_hand[i] && m_hand[i]->isCardClicked(mousePos, getCardPosition(i, alignRight)))
            return m_hand[i].get();
    }
    return nullptr;
}

sf::Vector2f Player::getCardPosition(size_t index, bool alignRight) const
{
    float xPos = alignRight ?
        (static_cast<float>(Config::WINDOW_WIDTH) - 20.f - Card::WIDTH - (index * CARD_SPACING)) :
        (20.f + (index * CARD_SPACING));

    return { xPos, BOTTOM_PANEL_TOP_Y + CARD_TOP_MARGIN };
}

void Player::endTurn()
{
    for (auto& monster : m_monsters)
    {
        if (monster && monster->isOnBoard())
            monster->resetActions();
    }

    // Once the per-game key cap has been fully handed out, stop granting
    // more each turn - m_keys can still be spent down and never regrows past this.
    if (m_maxKeys == 0) return;

    m_keys += 3;
    m_maxKeys = std::max(0, m_maxKeys - 3);
}

void Player::reduceKeys(int cost)
{
	m_keys -= cost;
}

Heart* Player::getHeart()
{
    return m_heart.get();
}

bool Player::isDead() const
{
    return !(m_heart->isAlive());
}

PlayerSide Player::getSide() const { return m_side; }

void Player::onTurnStart(Board& board) {}
void Player::updateTurn(Board& board) {}
bool Player::isBusy() const { return false; }
