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

    // Attack sprite sheet timing: the shared 6x4 grid (see
    // Monster::setStandardSpriteAnimations), timed against Barzilla's normal
    // (non-empowered) attack - a single FireBlast traveling for
    // FIRE_BLAST_DURATION = 0.5s, the same shape as Muffintop's single
    // MuffinShot. 24 frames at 0.02s finishes in ~0.48s, close to that 0.5s
    // window. (The empowered burst's own window is a bit shorter - not
    // specially timed for, since isAttacking() and this sheet are the same
    // regardless of which variant fired.)
    constexpr float ATTACK_FRAME_DURATION = 0.02f;
}

Barzilla::Barzilla(PlayerSide side)
    : Monster(side, BASE_HEALTH, BASE_ATTACK, BASE_RANGE, BASE_COOLDOWN, -1, -1, sf::Color::Red, "barzilla")
{
    setStandardSpriteAnimations("barzilla", "barzilla_walk", ATTACK_FRAME_DURATION);
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
void Barzilla::onSpecialAbility(const Board& board, BoardEntity* target)
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

std::unique_ptr<AttackAnimation> Barzilla::createAttackAnimation(sf::Vector2f targetPosition) const
{
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
        fireBlastTexture, m_screenPos, targetPosition, FIRE_BLAST_DURATION, FIRE_BLAST_THICKNESS);
}
