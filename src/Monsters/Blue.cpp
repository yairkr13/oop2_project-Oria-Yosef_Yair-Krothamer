#include "Monsters/Blue.h"
#include "Attacks/BurstProjectileAnimation.h"
#include "AssetsManager.h"
#include "Constants.h"
#include "Board.h"
#include "Tile.h"

namespace
{
    // Normal-attack burst (WindBlast): several rapid shots, staggered, only
    // the last one deals damage - see BurstProjectileAnimation.
    constexpr int WIND_BLAST_COUNT = 5;
    constexpr float WIND_BLAST_LAUNCH_INTERVAL = 0.08f; // seconds between successive launches
    constexpr float WIND_BLAST_TRAVEL_DURATION = 0.35f; // seconds each individual shot takes to arrive
    constexpr float WIND_BLAST_SIZE = Config::MONSTER_BOARD_SIZE * 0.35f; // smaller per-shot than a full monster, several fly together
}

Blue::Blue(PlayerSide side)
    : Monster(side, "Blue", BASE_HEALTH, BASE_ATTACK, BASE_RANGE, BASE_COOLDOWN, -1, -1, sf::Color::Magenta, "blue")
{
    //TextureManager::getInstance().loadTexture("blue_r", "resources/Monster/Blue/Blue_R.png");
    //TextureManager::getInstance().loadTexture("blue_l", "resources/Monster/Blue/Blue_L.png");
    //TextureManager::getInstance().loadTexture("blue_card_r", "resources/Monster/Blue/Blue_card_R.png");
    //TextureManager::getInstance().loadTexture("blue_card_l", "resources/Monster/Blue/Blue_card_L.png");
}

std::unique_ptr<AttackAnimation> Blue::createAttackAnimation(BoardEntity* target) const
{
    if (!target) return nullptr;

    const sf::Texture& windBlastTexture = AssetsManager::getInstance().getTexture("wind_blast");
    return std::make_unique<BurstProjectileAnimation>(
        windBlastTexture, m_screenPos, target->getScreenPosition(),
        WIND_BLAST_COUNT, WIND_BLAST_LAUNCH_INTERVAL, WIND_BLAST_TRAVEL_DURATION, WIND_BLAST_SIZE);
}

// Knockback: pushes the target up to 2 tiles directly away from Blue.
// "Backward" is defined as continuing along the attacker->target direction -
// since Knockback only ever targets an adjacent enemy (Blue's own
// BASE_RANGE is 1, reused unchanged for targeting), that direction is
// always exactly one of the board's six hex-neighbor offsets, so it falls
// straight out of coordinate subtraction with no separate direction table
// needed. Passability (including the existing Hole/flying rule) and
// occupancy are both reused as-is from Tile - Blue adds no new board rule,
// only the meaning of "knockback" itself.
void Blue::onSpecialAbility(Board& board, BoardEntity* target)
{
    Monster* targetMonster = target ? target->asMonster() : nullptr;
    if (!targetMonster) return;

    Tile* currentTile = targetMonster->getCurrentTile();
    if (!currentTile) return;

    int dq = targetMonster->getQ() - m_q;
    int dr = targetMonster->getRow() - m_row;

    Tile* step1 = board.getTileAt(targetMonster->getQ() + dq, targetMonster->getRow() + dr);
    bool step1Valid = step1 && !step1->hasEntity() && step1->isPassableFor(targetMonster);
    if (!step1Valid) return; // first tile blocked/off-board -> no movement at all

    Tile* step2 = board.getTileAt(targetMonster->getQ() + 2 * dq, targetMonster->getRow() + 2 * dr);
    bool step2Valid = step2 && !step2->hasEntity() && step2->isPassableFor(targetMonster);

    Tile* destination = step2Valid ? step2 : step1; // push 2 if both clear, otherwise exactly 1

    currentTile->clearEntity();
    destination->setEntity(targetMonster);
    targetMonster->spawnOnBoard(destination->getQ(), destination->getRow(),
        board.tileToScreen(destination->getQ(), destination->getRow()));
}