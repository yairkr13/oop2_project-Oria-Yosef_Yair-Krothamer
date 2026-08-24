#include "Monsters/Barzilla.h"
#include "Attacks/SplashAttackAnimation.h"
#include "Attacks/BurstProjectileAnimation.h"
#include "Attacks/GrowingEffectAnimation.h"
#include "AssetsManager.h"
#include "Constants.h"

namespace
{
    constexpr float FIRE_BLAST_DURATION = 0.5f; // seconds for the blast to travel from Barzilla to its target

    // Same reasoning as Mozzy's ACID_SPLASH_THICKNESS: kept proportional to
    // Config::MONSTER_BOARD_SIZE (the project's existing on-board sizing
    // reference) rather than FireBlast.png's raw pixel height.
    constexpr float FIRE_BLAST_THICKNESS = Config::MONSTER_BOARD_SIZE * 0.5f;

    // Empowered Attack's projectile: several FireBlast copies traveling
    // together with only a small stagger between them, reusing
    // BurstProjectileAnimation exactly as Blue/Henrietta already do for
    // their own normal attacks - only the texture/count/timing differ. The
    // short interval (vs. e.g. Blue's 0.08s) is what keeps the 3 copies
    // reading as one grouped burst rather than three separate shots.
    constexpr int EMPOWERED_FIRE_BLAST_COUNT = 3;
    constexpr float EMPOWERED_FIRE_BLAST_LAUNCH_INTERVAL = 0.04f;
    constexpr float EMPOWERED_FIRE_BLAST_TRAVEL_DURATION = 0.4f;
    constexpr float EMPOWERED_FIRE_BLAST_SIZE = Config::MONSTER_BOARD_SIZE * 0.55f;

    // Empowered Attack's impact flash: reuses GrowingEffectAnimation (built
    // for Henrietta's Protection shield) and the existing fire_blast
    // texture - a quick, prominent grow-and-fade on the target, so landing
    // an empowered hit visibly reads as stronger than a normal one. No
    // fade-in hold needed for an impact flash - it grows, sits for a
    // beat, then is gone.
    constexpr float EMPOWERED_IMPACT_SIZE = Config::MONSTER_BOARD_SIZE * 1.3f;
    constexpr float EMPOWERED_IMPACT_GROW_DURATION = 0.12f;
    constexpr float EMPOWERED_IMPACT_HOLD_DURATION = 0.08f;
    constexpr float EMPOWERED_IMPACT_FADE_DURATION = 0.15f;

    // Walking sprite sheet: 6 columns x 4 rows (24 frames total), read row
    // by row left-to-right - verified against the actual file, same layout
    // as Muffintop's working sheet. Same frame duration as Muffintop for a
    // consistent pace across monsters.
    constexpr int WALK_SHEET_COLUMNS = 6;
    constexpr int WALK_SHEET_ROWS = 4;
    constexpr float WALK_FRAME_DURATION = 0.06f;

    // Idle sprite sheet: 6x4 (24 frames), verified against the actual file -
    // same grid as every other sheet in the project, inspected directly
    // rather than assumed. Same calm pace as Muffintop's Idle.
    constexpr int IDLE_SHEET_COLUMNS = 6;
    constexpr int IDLE_SHEET_ROWS = 4;
    constexpr float IDLE_FRAME_DURATION = 0.08f;

    // Attack sprite sheet: same 6x4 grid, timed against Barzilla's normal
    // (non-empowered) attack - a single FireBlast traveling for
    // FIRE_BLAST_DURATION = 0.5s, the same shape as Muffintop's single
    // MuffinShot. 24 frames at 0.02s finishes in ~0.48s, close to that 0.5s
    // window. (The empowered burst's own window is a bit shorter - not
    // specially timed for, since isAttacking() and this sheet are the same
    // regardless of which variant fired.)
    constexpr int ATTACK_SHEET_COLUMNS = 6;
    constexpr int ATTACK_SHEET_ROWS = 4;
    constexpr float ATTACK_FRAME_DURATION = 0.02f;

    // Die sprite sheet: same 6x4 grid, non-looping (setDieSpriteAnimation
    // always configures looping=false - see Monster). Same pace as
    // Muffintop's Die: ~1.2s, a clearly readable one-shot collapse.
    constexpr int DIE_SHEET_COLUMNS = 6;
    constexpr int DIE_SHEET_ROWS = 4;
    constexpr float DIE_FRAME_DURATION = 0.05f;
}

Barzilla::Barzilla(PlayerSide side)
    : Monster(side, "Barzilla", BASE_HEALTH, BASE_ATTACK, BASE_RANGE, BASE_COOLDOWN, -1, -1, sf::Color::Red, "barzilla")
{
    setWalkAnimation("barzilla_walk", WALK_SHEET_COLUMNS, WALK_SHEET_ROWS, WALK_FRAME_DURATION);
    setIdleSpriteAnimation("barzilla_idle", IDLE_SHEET_COLUMNS, IDLE_SHEET_ROWS, IDLE_FRAME_DURATION);
    setAttackSpriteAnimation("barzilla_attack", ATTACK_SHEET_COLUMNS, ATTACK_SHEET_ROWS, ATTACK_FRAME_DURATION);
    setDieSpriteAnimation("barzilla_die", DIE_SHEET_COLUMNS, DIE_SHEET_ROWS, DIE_FRAME_DURATION);
}

// Empowered Attack: doubles the damage of the next successful attack only.
// The multiplier is read here, at the moment an attack actually resolves -
// since damage now applies at animation-impact time (see
// createAttackAnimation/Tile::receiveAttackFrom), this is naturally also
// exactly when the bonus is consumed, with no extra plumbing needed for
// that timing to line up. This is also the Special's actual commit point:
// arming it (onSpecialAbility, below) deliberately left the action/cooldown
// untouched, so committing them here - only once the empowered attack has
// genuinely happened - is what makes "select Special" and "use Special"
// two different events instead of one.
void Barzilla::attack(BoardEntity* target)
{
    bool empowered = m_empoweredAttack;
    int damage = empowered ? m_attackDamage * 2 : m_attackDamage;

    if (empowered)
    {
        m_empoweredAttack = false;
        m_specialCooldown = m_baseCooldown;

        // Stronger impact flash so an empowered hit visibly reads as
        // different from a normal one - played on the TARGET via the same
        // playSpecialAbilityAnimation slot Henrietta's shield/Mozzy's
        // freeze/Muffintop's heal already use for an incoming effect.
        // attack() only ever runs here as the animation's own onImpact
        // callback (see Board::performAction), so this fires at exactly
        // the same synchronized moment the grouped FireBlast burst
        // (createAttackAnimation, below) actually lands - the target is
        // never hit before the projectiles visually arrive.
        const sf::Texture& impactTexture = AssetsManager::getInstance().getTexture("fire_blast");
        auto impactFlash = std::make_unique<GrowingEffectAnimation>(
            impactTexture, target->getScreenPosition(), EMPOWERED_IMPACT_SIZE,
            EMPOWERED_IMPACT_GROW_DURATION, EMPOWERED_IMPACT_HOLD_DURATION, EMPOWERED_IMPACT_FADE_DURATION);
        target->playSpecialAbilityAnimation(std::move(impactFlash));
    }

    target->takeDamage(damage);
    useAction(); // an attack always costs an action, empowered or not
}

// Arms the bonus for the next attack - does not itself consume an action or
// touch the cooldown (see Monster::useSpecialAbility /
// specialAbilityCommitsOnSelect()). Selecting the Special is not using it.
void Barzilla::onSpecialAbility(Board& board, BoardEntity* target)
{
    m_empoweredAttack = true;
}

void Barzilla::onTurnBoundary()
{
    BoardEntity::onTurnBoundary();
    // Unconsumed bonus does not carry into a future turn.
    m_empoweredAttack = false;
}

std::unique_ptr<AttackAnimation> Barzilla::createAttackAnimation(BoardEntity* target) const
{
    if (!target) return nullptr;

    const sf::Texture& fireBlastTexture = AssetsManager::getInstance().getTexture("fire_blast");

    // Empowered Attack gets a visibly different projectile - several
    // FireBlast copies traveling together (reusing BurstProjectileAnimation
    // exactly as Blue/Henrietta's own normal attacks already do, just with
    // a much shorter stagger so they read as one grouped burst) - instead
    // of the normal single reveal-band. The normal attack (below) is
    // completely untouched.
    if (m_empoweredAttack)
    {
        return std::make_unique<BurstProjectileAnimation>(
            fireBlastTexture, m_screenPos, target->getScreenPosition(),
            EMPOWERED_FIRE_BLAST_COUNT, EMPOWERED_FIRE_BLAST_LAUNCH_INTERVAL,
            EMPOWERED_FIRE_BLAST_TRAVEL_DURATION, EMPOWERED_FIRE_BLAST_SIZE);
    }

    // Normal attack - unchanged: "grows/reveals from attacker toward
    // target" mechanism, same as Mozzy's acid splash, reusing
    // SplashAttackAnimation as-is rather than duplicating it.
    return std::make_unique<SplashAttackAnimation>(
        fireBlastTexture, m_screenPos, target->getScreenPosition(), FIRE_BLAST_DURATION, FIRE_BLAST_THICKNESS);
}
