#pragma once
#include "BoardEntity.h"
#include "TextureManager.h"

class Heart : public BoardEntity {
public:
    // הבנאי של הלב קורא לבנאי של BoardEntity ומעביר לו את המיקומים
    Heart(PlayerSide side, int q, int row, const sf::Vector2f& position)
		: BoardEntity(q, row, position, 100), m_side(side), m_sprite(TextureManager::getInstance().get<sf::Texture>("heart100"))
    {
        const sf::Texture& texture = m_sprite.getTexture();

        // הגדרת נקודת המרכז לאמצע התמונה (עם סוגריים מסולסלים)
        m_sprite.setOrigin({ texture.getSize().x / 2.f, texture.getSize().y / 2.f });

        // חישוב ושמירת קנה המידה (Scale)
        float desiredSize = 60.f; // אתה יכול להחליף כאן ל-Config::MONSTER_BOARD_SIZE אם אתה רוצה שהלב יהיה בגודל של מפלצת
        float maxTextureDim = std::max(static_cast<float>(texture.getSize().x), static_cast<float>(texture.getSize().y));
        float scale = desiredSize / maxTextureDim;

        m_sprite.setScale({ scale, scale });

        // מיקום על המסך
        m_sprite.setPosition(m_screenPos);
    }

    void spawnOnBoard(int q, int row, const sf::Vector2f& screenPos) override
    {
        BoardEntity::spawnOnBoard(1,row,screenPos);
        m_sprite.setPosition(screenPos);
    }

    void draw(sf::RenderWindow& window, PlayerSide currentTurnSide) const override {
        window.draw(m_sprite);
        drawHealthBar(window);
    }

    //void takeDamage(int damage) override { m_health -= damage; }
 //   int getHealth() const override { return m_health; }
    PlayerSide getSide() const override { return m_side; }
    EntityType getType() const override { return EntityType::Heart; }

private:
    //int m_health;
    PlayerSide m_side;
    sf::Sprite m_sprite;
};