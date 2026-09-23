#include "Heart.h"
#include <limits>

Heart::Heart(PlayerSide side, int q, int row, const sf::Vector2f& position)
    : BoardEntity(q, row, position, 100), m_side(side),
    m_sprite(AssetsManager::getInstance().getTexture(textureKeyFor(side))),
    m_animation(AssetsManager::getInstance().getTexture(textureKeyFor(side)),
        SHEET_COLUMNS, SHEET_ROWS, FRAME_DURATION, DISPLAY_SIZE)
{
    // Sets m_sprite's texture rect/origin to frame 0 - SpriteSheet's own
    // job (see SpriteSheet::applyCurrentFrame), not something this class
    // computes itself. update()/draw() below keep this in sync every frame.
    m_animation.applyCurrentFrame(m_sprite);

    // Scaled against ONE frame's real pixel size (see
    // SpriteSheet::getBaseScale), not the whole sheet - the same on-board
    // size (DISPLAY_SIZE) the old single-image heart used.
    float scale = m_animation.getBaseScale();
    m_sprite.setScale({ scale, scale });

    m_sprite.setPosition(m_screenPos);
}

void Heart::spawnOnBoard(int q, int row, const sf::Vector2f& screenPos)
{
    BoardEntity::spawnOnBoard(q,row,screenPos);
    m_sprite.setPosition(screenPos);
}

void Heart::update(float dt)
{
    m_animation.update(dt);
}

void Heart::draw(sf::RenderWindow& window, PlayerSide currentSide) const {
    m_animation.applyCurrentFrame(m_sprite);
    window.draw(m_sprite);
    drawHealthBar(window);
}

PlayerSide Heart::getSide() const { return m_side; }

// Always outranks any Monster's own scoreAsAttackTarget() (bounded,
// since it's based on current HP) - the Heart is always the AI's top
// attack priority once reachable, regardless of its own current HP.
float Heart::scoreAsAttackTarget() const { return std::numeric_limits<float>::infinity(); }

const std::string& Heart::textureKeyFor(PlayerSide side)
{
    static const std::string blue = "heart_blue";
    static const std::string orange = "heart_orange";
    return side == PlayerSide::Left ? blue : orange;
}
