#include "Player.h"

std::string Player::handleCardClick(sf::Vector2f mousePos)
{
	for (auto& card : m_cards)
	{
		// נניח שלמחלקה Card יש פונקציה שמחזירה את שטח הפגיעה שלה
		if (card.getBounds().contains(mousePos))
		{
			return card.getMonsterName(); // מצאנו את הקלף שנלחץ!
		}
	}
	return nullptr; // לא לחצו על אף קלף
}

void Player::handleClick(const sf::Vector2f& pos)
{
	// Check if the click is within any of the cards' bounds
	for (const auto& card : m_cards)
	{
		if (card && card->getBound().contains(pos))
		{
			// Handle card click logic here
			return;
		}
		if (card->monsterClicked(pos))
		{
			// Handle monster click logic here
			return;
		}
	}
	// Click was not handled by any interactive element
}

//bool Player::handleClick(const sf::Vector2f& pos)
//{
//	// Check if the click is within the heart's bounds
//	if (m_heart && m_heart->getBound().contains(pos))
//	{
//		// Handle heart click logic here
//		return true;
//	}
	// Check if the click is within any of the cards' bounds
	//for (const auto& card : m_cards)
	//{
	//	if (card && card->getBound().contains(pos))
	//	{
	//		// Handle card click logic here

	//		return true;
	//	}
	//}
	//// Click was not handled by any interactive element
	//return false;
}


//#include <cmath>
//
//namespace { constexpr float DEG_TO_RAD = 3.14159265f / 180.f; }
//
//Player::Player(const sf::Texture& texNormal, const sf::Texture& texSmoke,
//               const sf::Texture* texReverse)
//    : m_sprite(texNormal)
//    , m_texNormal(&texNormal)
//    , m_texSmoke(&texSmoke)
//    , m_texReverse(texReverse)
//{
//    const auto bounds = m_sprite.getLocalBounds();
//    m_sprite.setOrigin({ bounds.size.x / 2.f, bounds.size.y / 2.f });
//    m_sprite.setScale({ 0.15f, 0.15f });
//    m_sprite.setPosition({ 400.f, 300.f });
//}
//
//void Player::update(float dt)
//{
//    using Key = sf::Keyboard::Key;
//
//    const float rad = m_angleDeg * DEG_TO_RAD;
//    const sf::Vector2f forward{ std::cos(rad), std::sin(rad) };
//    const bool movingForward = sf::Keyboard::isKeyPressed(Key::Up);
//
//    if (movingForward)
//    {
//        m_sprite.move(forward * SPEED * dt);
//        if (sf::Keyboard::isKeyPressed(Key::Left))  m_angleDeg -= ROTATION_SPEED * dt;
//        if (sf::Keyboard::isKeyPressed(Key::Right)) m_angleDeg += ROTATION_SPEED * dt;
//
//        // Advance smoke animation timer
//        m_animTimer += dt;
//        if (m_animTimer >= ANIM_INTERVAL)
//        {
//            m_animTimer -= ANIM_INTERVAL;
//            m_showSmoke = !m_showSmoke;
//        }
//    }
//    else
//    {
//        // Not moving forward — reset to normal texture immediately
//        m_showSmoke  = false;
//        m_animTimer  = 0.f;
//    }
//
//    if (sf::Keyboard::isKeyPressed(Key::Down))
//    {
//        m_sprite.move(-forward * SPEED * dt);
//        if (sf::Keyboard::isKeyPressed(Key::Left))  m_angleDeg -= ROTATION_SPEED * dt;
//        if (sf::Keyboard::isKeyPressed(Key::Right)) m_angleDeg += ROTATION_SPEED * dt;
//    }
//
//    m_sprite.setRotation(sf::degrees(m_angleDeg));
//
//    // Swap texture based on animation state (no reload — just pointer swap)
//    const sf::Texture* desired = nullptr;
//    if (sf::Keyboard::isKeyPressed(Key::Down) && m_texReverse)
//        desired = m_texReverse;
//    else if (m_texNormal && m_texSmoke)
//        desired = m_showSmoke ? m_texSmoke : m_texNormal;
//
//    if (desired)
//        m_sprite.setTexture(*desired);
//}
//
//void Player::draw(sf::RenderWindow& window) const
//{
//    window.draw(m_sprite);
//}
//
//const sf::Vector2f& Player::getPosition() const
//{
//    return m_sprite.getPosition();
//}
