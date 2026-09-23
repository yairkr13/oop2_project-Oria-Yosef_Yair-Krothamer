#include "Monsters/Blue.h"
#include "Attacks/BurstProjectileAnimation.h"
#include "Attacks/SpinningProjectileAnimation.h"
#include "AssetsManager.h"
#include "Constants.h"
#include "Board.h"
#include "Tiles/Tile.h"
#include "HexGrid.h"

namespace
{
    // Normal attack (WindBlast): a staggered burst of shots; only the
    // last one deals damage - see BurstProjectileAnimation.
    constexpr int WIND_BLAST_COUNT = 5;
    constexpr float WIND_BLAST_LAUNCH_INTERVAL = 0.08f; // seconds between successive launches
    constexpr float WIND_BLAST_TRAVEL_DURATION = 0.35f; // seconds each individual shot takes to arrive
    constexpr float WIND_BLAST_SIZE = Config::MONSTER_BOARD_SIZE * 0.35f; // smaller per-shot than a full monster, several fly together

    // Knockback wind effect travels along the knockback direction, from
    // roughly one tile before the target's position to half a tile past it.
    constexpr float WIND_EFFECT_APPROACH_TILES = 1.0f;
    constexpr float WIND_EFFECT_OVERSHOOT_TILES = 0.5f;
    constexpr float WIND_EFFECT_DURATION = 0.4f;
    constexpr float WIND_EFFECT_SIZE = Config::MONSTER_BOARD_SIZE * 0.9f;

    // Distance Knockback pushes the target (see Board::applyKnockback).
    constexpr int KNOCKBACK_DISTANCE_TILES = 2;

    // Attack sheet timing: sized to cover the burst's full ~0.67s flight
    // window (5 staggered shots plus travel time).
    constexpr float ATTACK_FRAME_DURATION = 0.028f;
}

Blue::Blue(PlayerSide side)
    : Monster(side, BASE_HEALTH, BASE_ATTACK, BASE_RANGE, BASE_COOLDOWN, -1, -1, sf::Color::Magenta, "blue")
{
    // Flying monster: uses its fly sheet instead of a ground walk cycle.
    setStandardSpriteAnimations("blue", "blue_fly", ATTACK_FRAME_DURATION);
}

std::unique_ptr<AttackAnimation> Blue::createAttackAnimation(sf::Vector2f targetPosition) const
{
    const sf::Texture& windBlastTexture = AssetsManager::getInstance().getTexture("wind_blast");
    return std::make_unique<BurstProjectileAnimation>(
        windBlastTexture, m_screenPos, targetPosition,
        WIND_BLAST_COUNT, WIND_BLAST_LAUNCH_INTERVAL, WIND_BLAST_TRAVEL_DURATION, WIND_BLAST_SIZE);
}

// Pushes the target up to 2 tiles along the attacker-to-target hex
// direction (resolved via HexGrid::stepToward); reuses Tile's existing
// passability/occupancy rules.
void Blue::onSpecialAbility(const Board& board, BoardEntity* target)
{
    // No target, or a target that's no longer a legal Special target (e.g. mid-death-animation).
	if (!target || !target->canBeTargetedBySpecial()) return;

    auto [dq, dr] = HexGrid::stepToward(m_q, m_row, target->getQ(), target->getRow());

    // Built from the target's original position/direction, before any
    // repositioning; plays regardless of whether the push itself is blocked.
    sf::Vector2f originalScreenPos = target->getScreenPosition();
    sf::Vector2f stepVector = board.tileToScreen(target->getQ() + dq, target->getRow() + dr)
        - board.tileToScreen(target->getQ(), target->getRow());

    sf::Vector2f windOrigin = originalScreenPos - stepVector * WIND_EFFECT_APPROACH_TILES;
    sf::Vector2f windTarget = originalScreenPos + stepVector * WIND_EFFECT_OVERSHOOT_TILES;

    const sf::Texture& windEffectTexture = AssetsManager::getInstance().getTexture("wind_effect");
    auto windEffect = std::make_unique<SpinningProjectileAnimation>(
        windEffectTexture, windOrigin, windTarget, WIND_EFFECT_DURATION, WIND_EFFECT_SIZE, 0.f);

    // Deferred to the wind's impact, so the target visually flies back
    // only once the wind "hits".
    windEffect->setOnImpact([&board, target, dq, dr]() {
        board.applyKnockback(target, dq, dr, KNOCKBACK_DISTANCE_TILES);
    });

    target->playSpecialAbilityAnimation(std::move(windEffect));
}

bool Blue::specialAbilityNeedsTarget() const
{
    return true;
}

std::string Blue::getSpecialAbilityDescription() const
{
    return "Knockback: Knocks back a targeted enemy monster on the grid.";
}

sf::Color Blue::getSpecialTargetHighlightColor() const
{
    return sf::Color(60, 120, 255, 180); // blue
}
