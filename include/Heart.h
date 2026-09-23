#pragma once
#include "BoardEntity.h"
#include "AssetsManager.h"
#include "SpriteSheet.h"
#include <string>

class Heart : public BoardEntity {
public:
    Heart(PlayerSide side, int q, int row, const sf::Vector2f& position);

    void spawnOnBoard(int q, int row, const sf::Vector2f& screenPos) override;

    // Advances the heartbeat loop - SpriteSheet defaults to looping, so this
    // never needs to check isFinished()/reset() itself.
    void update(float dt) override;

    void draw(sf::RenderWindow& window, PlayerSide currentSide) const override;

    PlayerSide getSide() const override;

    // Always outranks any Monster's own scoreAsAttackTarget() (bounded,
    // since it's based on current HP) - the Heart is always the AI's top
    // attack priority once reachable, regardless of its own current HP.
    float scoreAsAttackTarget() const override;

private:
    // Left plays BlueHeart.png, Right plays OrangeHeart.png - the only
    // per-side difference a Heart has.
    static const std::string& textureKeyFor(PlayerSide side);

    // BlueHeart.png/OrangeHeart.png's own grid - 6x2 (12 frames), distinct
    // from every monster sprite sheet's 6x4/24 frames (see
    // Monster::setStandardSpriteAnimations) - SpriteSheet takes both as
    // plain parameters, so the two coexist with no shared assumption
    // anywhere about frame count or layout.
    static constexpr int SHEET_COLUMNS = 6;
    static constexpr int SHEET_ROWS = 2;
    static constexpr float FRAME_DURATION = 0.08f;

    // On-screen size - same value the old static heart used, not tied to
    // Config::MONSTER_BOARD_SIZE since a Heart isn't a monster.
    static constexpr float DISPLAY_SIZE = 60.f;

    PlayerSide m_side;

    // mutable: draw() is const (see BoardEntity's interface), but applying
    // the current frame (see SpriteSheet::applyCurrentFrame) mutates the
    // sprite's texture/rect/origin - the same reasoning Monster::m_sprite
    // already relies on.
    mutable sf::Sprite m_sprite;

    // A Heart has exactly one animation state that's always playing, unlike
    // Monster's several mutually-exclusive states - a single SpriteSheet is
    // enough; SpriteAnimator's priority/state-selection machinery would have
    // nothing to do here.
    SpriteSheet m_animation;
};
