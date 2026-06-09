#pragma once
#include <SFML/Graphics.hpp>

class Player
{
public:
    // Takes both textures. Player stores const references — textures must
    // outlive the Player (Game owns them in m_textures, so this is fine).
    Player(const sf::Texture& texNormal, const sf::Texture& texSmoke,
           const sf::Texture* texReverse = nullptr);
    Player() = default;

    void update(float dt);
    void draw(sf::RenderWindow& window) const;
    const sf::Vector2f& getPosition() const;

private:
    sf::Sprite m_sprite;
    float m_angleDeg  = 0.f;

    // Animation state
    const sf::Texture* m_texNormal = nullptr;   // car_blue.png
    const sf::Texture* m_texSmoke   = nullptr;   // car_blue_smoke.png
    const sf::Texture* m_texReverse = nullptr;   // car_blue_reverse.png
    float m_animTimer   = 0.f;
    bool  m_showSmoke   = false;

    static constexpr float ANIM_INTERVAL = 0.12f;  // seconds per frame (~120 ms)
    static constexpr float SPRITE_SCALE   = 0.15f;
    static constexpr float SPEED          = 180.f;
    static constexpr float ROTATION_SPEED = 120.f;
};
