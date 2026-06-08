#include "Player.h"
#include <cmath>

namespace
{
    constexpr float DEG_TO_RAD = 3.14159265f / 180.f;
}

Player::Player(const sf::Texture& texture):m_sprite(texture)
{
    //m_shape.setSize({ 80.f, 40.f });
    m_sprite.setOrigin({ 40.f, 20.f }); // center origin, so rotation looks natural
    //m_sprite.setTexture(m_textures["player"]);
    m_sprite.setPosition({ 400.f, 300.f });
}

void Player::update(float dt)
{
    using Key = sf::Keyboard::Key;


    const float rad = m_angleDeg * DEG_TO_RAD;
    const sf::Vector2f forward{ std::cos(rad), std::sin(rad) };

    if (sf::Keyboard::isKeyPressed(Key::Up))
    {
        m_sprite.move(forward * SPEED * dt);
        if (sf::Keyboard::isKeyPressed(Key::Left))
            m_angleDeg -= ROTATION_SPEED * dt;

        if (sf::Keyboard::isKeyPressed(Key::Right))
            m_angleDeg += ROTATION_SPEED * dt;
    }

    if (sf::Keyboard::isKeyPressed(Key::Down))
    {
        m_sprite.move(-forward * SPEED * dt);
        if (sf::Keyboard::isKeyPressed(Key::Left))
            m_angleDeg -= ROTATION_SPEED * dt;

        if (sf::Keyboard::isKeyPressed(Key::Right))
            m_angleDeg += ROTATION_SPEED * dt;
    }

    m_sprite.setRotation(sf::degrees(m_angleDeg));
}

void Player::draw(sf::RenderWindow& window) const
{
    window.draw(m_sprite);
}

const sf::Vector2f& Player::getPosition() const
{
    return m_sprite.getPosition();
}