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

std::unique_ptr<AttackAnimation> Barzilla::createAttackAnimation(sf::Vector2f targetPosition) const
{
    const sf::Texture& fireBlastTexture = AssetsManager::getInstance().getTexture("fire_blast");

    // Normal attack - unchanged: "grows/reveals from attacker toward
    // target" mechanism, same as Mozzy's acid splash, reusing
    // SplashAttackAnimation as-is rather than duplicating it.
    return std::make_unique<SplashAttackAnimation>(
        fireBlastTexture, m_screenPos, targetPosition, FIRE_BLAST_DURATION, FIRE_BLAST_THICKNESS);
}
