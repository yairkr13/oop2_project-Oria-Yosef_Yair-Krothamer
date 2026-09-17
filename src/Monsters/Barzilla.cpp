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

    // Old empowered-projectile constants, kept as a comment - Barzilla's own
    // attack no longer varies visually by m_empoweredAttack (see
    // createAttackAnimation below):
    //
    // constexpr int EMPOWERED_FIRE_BLAST_COUNT = 3;
    // constexpr float EMPOWERED_FIRE_BLAST_LAUNCH_INTERVAL = 0.04f;
    // constexpr float EMPOWERED_FIRE_BLAST_TRAVEL_DURATION = 0.4f;
    // constexpr float EMPOWERED_FIRE_BLAST_SIZE = Config::MONSTER_BOARD_SIZE * 0.55f;

    // Empowered Attack's "grant" visual, played on the ALLY the instant
    // Barzilla empowers them (see Barzilla::onSpecialAbility) - repurposed
    // from what used to be this same GrowingEffectAnimation/fire_blast
    // texture playing as an impact flash on a landed empowered hit. Still a
    // quick, prominent grow-and-fade - now reading as "this ally now glows
    // with Barzilla's fire" instead. No fade-in hold needed - it grows,
    // sits for a beat, then is gone.
    constexpr float EMPOWERED_IMPACT_SIZE = Config::MONSTER_BOARD_SIZE * 1.3f;
    constexpr float EMPOWERED_IMPACT_GROW_DURATION = 0.12f;
    constexpr float EMPOWERED_IMPACT_HOLD_DURATION = 0.08f;
    constexpr float EMPOWERED_IMPACT_FADE_DURATION = 0.15f;

    // Empowered Attack's own damage multiplier (see
    // BoardEntity::applyEmpoweredAttack/Monster::attack(), which are purely
    // mechanical and take this as a plain parameter - Barzilla's own balance
    // number belongs here, not hardcoded inside Monster).
    constexpr float EMPOWERED_ATTACK_MULTIPLIER = 2.f;

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

// Old self-buff version, kept as a comment (not deleted) per request - see
// Barzilla.h for why this no longer applies now that Empowered Attack
// targets an ally instead of arming Barzilla's own next attack:
//
// void Barzilla::attack(BoardEntity* target)
// {
//     bool empowered = m_empoweredAttack;
//     int damage = empowered ? m_attackDamage * 2 : m_attackDamage;
//     if (empowered)
//     {
//         m_empoweredAttack = false;
//         m_specialCooldown = m_baseCooldown;
//         const sf::Texture& impactTexture = AssetsManager::getInstance().getTexture("fire_blast");
//         auto impactFlash = std::make_unique<GrowingEffectAnimation>(
//             impactTexture, target->getScreenPosition(), EMPOWERED_IMPACT_SIZE,
//             EMPOWERED_IMPACT_GROW_DURATION, EMPOWERED_IMPACT_HOLD_DURATION, EMPOWERED_IMPACT_FADE_DURATION);
//         target->playSpecialAbilityAnimation(std::move(impactFlash));
//     }
//     target->takeDamage(damage);
//     useAction();
// }

// Empowered Attack: grants the chosen ally's next attack double damage -
// see BoardEntity::applyEmpoweredAttack/Monster::attack() for where that's
// actually consumed (on the ally's own attack, not Barzilla's). Commits
// immediately, like every other Special now (base
// Monster::useSpecialAbility already handles the action/cooldown
// bookkeeping - nothing extra needed here).
//
// The visual is played on the TARGET at the moment the buff is granted
// (same "grow" effect the old self-buff version played at impact time,
// reused here as the moment Barzilla's fire empowers his ally instead) -
// same playSpecialAbilityAnimation slot Henrietta's shield/Mozzy's
// freeze/Muffintop's heal already use for an incoming effect.
void Barzilla::onSpecialAbility(Board& board, BoardEntity* target)
{
    if (!target) return;

    target->applyEmpoweredAttack(EMPOWERED_ATTACK_MULTIPLIER);

    const sf::Texture& impactTexture = AssetsManager::getInstance().getTexture("fire_blast");
    auto empowerEffect = std::make_unique<GrowingEffectAnimation>(
        impactTexture, target->getScreenPosition(), EMPOWERED_IMPACT_SIZE,
        EMPOWERED_IMPACT_GROW_DURATION, EMPOWERED_IMPACT_HOLD_DURATION, EMPOWERED_IMPACT_FADE_DURATION);
    target->playSpecialAbilityAnimation(std::move(empowerEffect));
}

// Old onTurnBoundary override, kept as a comment - see Barzilla.h for why
// it's no longer needed (the buff now lives on the ally, not on Barzilla):
//
// void Barzilla::onTurnBoundary()
// {
//     BoardEntity::onTurnBoundary();
//     m_empoweredAttack = false;
// }

std::unique_ptr<AttackAnimation> Barzilla::createAttackAnimation(BoardEntity* target) const
{
    if (!target) return nullptr;

    const sf::Texture& fireBlastTexture = AssetsManager::getInstance().getTexture("fire_blast");

    // Old empowered-burst branch, kept as a comment - Barzilla's own attack
    // no longer varies by m_empoweredAttack (that flag no longer lives on
    // Barzilla at all - see Monster.h):
    //
    // if (m_empoweredAttack)
    // {
    //     return std::make_unique<BurstProjectileAnimation>(
    //         fireBlastTexture, m_screenPos, target->getScreenPosition(),
    //         EMPOWERED_FIRE_BLAST_COUNT, EMPOWERED_FIRE_BLAST_LAUNCH_INTERVAL,
    //         EMPOWERED_FIRE_BLAST_TRAVEL_DURATION, EMPOWERED_FIRE_BLAST_SIZE);
    // }

    // Normal attack - unchanged: "grows/reveals from attacker toward
    // target" mechanism, same as Mozzy's acid splash, reusing
    // SplashAttackAnimation as-is rather than duplicating it.
    return std::make_unique<SplashAttackAnimation>(
        fireBlastTexture, m_screenPos, target->getScreenPosition(), FIRE_BLAST_DURATION, FIRE_BLAST_THICKNESS);
}
