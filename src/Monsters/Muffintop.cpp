#include "Monsters/Muffintop.h"
#include "Attacks/SpinningProjectileAnimation.h"
#include "Attacks/RisingEffectAnimation.h"
#include "AssetsManager.h"
#include "Constants.h"

namespace
{
    constexpr float MUFFIN_SHOT_DURATION = 0.5f; // seconds for the muffin to fly from Muffintop to its target

    // On-screen size of the flying muffin, kept proportional to
    // Config::MONSTER_BOARD_SIZE - the same reference constant Monster's
    // own sprite is scaled against - rather than MuffinShot.png's raw pixel
    // size, so it reads as a projectile next to a full-size monster instead
    // of overwhelming it.
    constexpr float MUFFIN_SHOT_SIZE = Config::MONSTER_BOARD_SIZE * 0.6f;

    // Heal effect: rises the full height of a tile (bottom to top), sized
    // similarly to the other effect animations, over a gentler duration
    // than the sharper attack effects - a heal is meant to read as a slow
    // glow, not an impact. Slowed down further from the original single-sprite
    // timing so the player has time to actually notice it.
    constexpr float HEAL_EFFECT_DURATION = 0.9f;
    constexpr float HEAL_EFFECT_SIZE = Config::MONSTER_BOARD_SIZE * 0.7f;
    constexpr float HEAL_EFFECT_RISE_DISTANCE = 2.f * Config::TILE_RADIUS;

    // Three copies (left/center/right) instead of one, so the heal reads as
    // a single wide bloom around the healed monster rather than one narrow
    // beam. Spacing is kept well under HEAL_EFFECT_SIZE so the three overlap
    // visually and stay one effect, not three separate ones; the small
    // stagger gives them a gentle ripple instead of rising in lockstep.
    constexpr int HEAL_EFFECT_INSTANCE_COUNT = 3;
    constexpr float HEAL_EFFECT_HORIZONTAL_SPACING = Config::MONSTER_BOARD_SIZE * 0.4f;
    constexpr float HEAL_EFFECT_STAGGER_DELAY = 0.06f;

    // Walking sprite sheet: 6 columns x 4 rows (24 frames total), read row
    // by row left-to-right. frameDuration is the one knob for animation
    // speed - this value shows the full 24-frame cycle in ~1.44s, looping
    // continuously for as long as Muffintop keeps moving.
    constexpr int WALK_SHEET_COLUMNS = 6;
    constexpr int WALK_SHEET_ROWS = 4;
    constexpr float WALK_FRAME_DURATION = 0.06f;

    // Attack sprite sheet: same 6x4 (24-frame) grid, verified against the
    // actual file. Frame duration is chosen (unlike the walk sheet's) so
    // the full 24-frame cycle completes in ~0.48s - close to
    // MUFFIN_SHOT_DURATION (0.5s) above - so Muffintop's own windup-to-throw
    // motion finishes right around when the projectile actually lands,
    // instead of being cut off mid-cycle or finishing early and holding on
    // the last frame. isAttacking() (see Monster::update()/draw()) is what
    // drives this - already true for exactly the projectile's flight time,
    // so no extra timing state was needed here.
    constexpr int ATTACK_SHEET_COLUMNS = 6;
    constexpr int ATTACK_SHEET_ROWS = 4;
    constexpr float ATTACK_FRAME_DURATION = 0.02f;

    // Idle sprite sheet: same 6x4 (24-frame) grid, verified against the
    // actual file - Muffintop_Idle.png happens to share Walk/Attack's exact
    // pixel dimensions (5760x3840), but that's coincidence, not an
    // assumption; the layout was inspected directly rather than copied.
    // frameDuration is deliberately the slowest of the three (Attack
    // 0.02s, Walk 0.06s, Idle here) - full cycle ~1.92s - so idling reads
    // as a calm, subtle sway rather than the brisker walk cycle, since this
    // is what plays for as long as Muffintop just sits on the board.
    constexpr int IDLE_SHEET_COLUMNS = 6;
    constexpr int IDLE_SHEET_ROWS = 4;
    constexpr float IDLE_FRAME_DURATION = 0.08f;

    // Die sprite sheet: same 6x4 (24-frame) grid, verified against the
    // actual file - again coincidentally matching Walk/Attack/Idle's pixel
    // dimensions (5760x3840), inspected directly rather than assumed. Plays
    // once (setDieSpriteAnimation always configures a non-looping sheet -
    // see Monster/SpriteSheet) at a brisker pace than Idle: full
    // 24-frame cycle in ~1.2s, a clearly readable collapse without
    // lingering once the monster is already dead and gone from play.
    constexpr int DIE_SHEET_COLUMNS = 6;
    constexpr int DIE_SHEET_ROWS = 4;
    constexpr float DIE_FRAME_DURATION = 0.05f;
}

Muffintop::Muffintop(PlayerSide side)
    : Monster(side, "Muffintop", BASE_HEALTH, BASE_ATTACK, BASE_RANGE, BASE_COOLDOWN, -1, -1, sf::Color::Magenta, "muffintop")
{
    // Test case for the sprite-sheet-animation mechanism (see
    // Monster::setWalkAnimation/setAttackSpriteAnimation/
    // setIdleSpriteAnimation/setDieSpriteAnimation) - Muffintop only, for
    // now.
    setWalkAnimation("muffintop_walk", WALK_SHEET_COLUMNS, WALK_SHEET_ROWS, WALK_FRAME_DURATION);
    setAttackSpriteAnimation("muffintop_attack", ATTACK_SHEET_COLUMNS, ATTACK_SHEET_ROWS, ATTACK_FRAME_DURATION);
    setIdleSpriteAnimation("muffintop_idle", IDLE_SHEET_COLUMNS, IDLE_SHEET_ROWS, IDLE_FRAME_DURATION);
    setDieSpriteAnimation("muffintop_die", DIE_SHEET_COLUMNS, DIE_SHEET_ROWS, DIE_FRAME_DURATION);
}


// Heal Ally: the Special commits here (Monster::useSpecialAbility has
// already consumed the action and reset the cooldown by the time this
// runs), but the actual +25% max-HP heal is deferred to the moment the
// HealEffect animation finishes rising - not applied immediately. The
// effect is created here (Muffintop knows its own ability's texture/style)
// but handed to the TARGET to own/update/draw (playSpecialAbilityAnimation),
// exactly mirroring how an attacker hands an attack animation to itself:
// here the effect's owner is whichever entity it visually belongs to, and
// that's the healed ally's own tile, not Muffintop.
void Muffintop::onSpecialAbility(Board& board, BoardEntity* target)
{
    Monster* targetMonster = target ? target->asMonster() : nullptr;
    if (!targetMonster) return;

    const sf::Texture& healEffectTexture = AssetsManager::getInstance().getTexture("heal_effect");

    // Starts at the bottom of the target's own tile and rises to the top -
    // getScreenPosition() is the tile's center, so the bottom is one
    // TILE_RADIUS below it.
    sf::Vector2f bottomOfTile = targetMonster->getScreenPosition() + sf::Vector2f(0.f, Config::TILE_RADIUS);

    auto healEffect = std::make_unique<RisingEffectAnimation>(
        healEffectTexture, bottomOfTile, HEAL_EFFECT_RISE_DISTANCE, HEAL_EFFECT_DURATION, HEAL_EFFECT_SIZE,
        HEAL_EFFECT_INSTANCE_COUNT, HEAL_EFFECT_HORIZONTAL_SPACING, HEAL_EFFECT_STAGGER_DELAY);

    // heal() (BoardEntity) still does the actual +HP and max-HP clamp -
    // nothing here duplicates that math, it only decides *when* to call it.
    healEffect->setOnImpact([targetMonster]() {
        targetMonster->heal(static_cast<int>(targetMonster->getMaxHealth() * 0.25f));
    });

    targetMonster->playSpecialAbilityAnimation(std::move(healEffect));
}

std::unique_ptr<AttackAnimation> Muffintop::createAttackAnimation(BoardEntity* target) const
{
    if (!target) return nullptr;

    // Genuinely different mechanism from Mozzy/Barzilla's splash-reveal:
    // the muffin itself flies from Muffintop to the target while spinning,
    // so this uses SpinningProjectileAnimation instead of SplashAttackAnimation.
    const sf::Texture& muffinShotTexture = AssetsManager::getInstance().getTexture("muffin_shot");
    return std::make_unique<SpinningProjectileAnimation>(
        muffinShotTexture, m_screenPos, target->getScreenPosition(), MUFFIN_SHOT_DURATION, MUFFIN_SHOT_SIZE);
}