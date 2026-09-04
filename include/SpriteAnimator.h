#pragma once
#include "SpriteSheet.h"
#include <functional>
#include <memory>
#include <vector>

// A generic, reusable sprite-sheet-based animation state machine. Sits one
// layer above SpriteSheet/FrameAnimation (which know how to play ONE
// timeline) - this class knows how to hold SEVERAL named timelines and
// decide, every frame, which single one of them is currently showing.
//
// Deliberately knows nothing about monsters, movement, attacking, dying, or
// any other gameplay concept - same spirit as SpriteSheet itself. A state is
// just: an opaque caller-chosen int id, a SpriteSheet, a predicate the owner
// supplies to say "is this state active right now", and a priority used to
// break ties when more than one predicate is true at once. This class never
// interprets what any id or predicate MEANS - the owner (e.g. Monster)
// decides that entirely; this class only ever answers "given what you told
// me, who's active, and what's their current frame."
//
// Typical use (see Monster::setWalkAnimation and friends for the real
// thing): an owner registers each of its states once, in its constructor,
// via addState(); every frame it calls update(dt) once, then draw()-time
// asks applyCurrentFrame()/getActiveBaseScale() for whatever is active now.
// The owner never needs its own if/else priority chain, and never needs to
// keep a separate "which one is active" decision in sync between its own
// update and draw - this class is the one place that decision is made.
class SpriteAnimator
{
public:
    // Registers one animation state. `id` is an opaque, caller-defined
    // value (e.g. a scoped enum cast to int) - this class only ever
    // compares it for equality (see isStateFinished/getActiveStateId),
    // never interprets it. `sheet` is this state's own SpriteSheet -
    // ownership moves in; this class drives it (update/reset) from here
    // on. `isActive` is a predicate the owner supplies (typically a lambda
    // capturing the owner's own state) deciding whether this state should
    // be showing right now. `priority` breaks ties when more than one
    // state's predicate is true simultaneously: the LOWEST priority value
    // wins (checked first) - e.g. a "Die" state registered with a lower
    // priority than "Walk" keeps winning for as long as both predicates
    // happen to hold. States are kept internally sorted by priority, so
    // registration order itself never matters - callers (e.g. each
    // Monster subclass's constructor) are free to register states in any
    // order without affecting which one wins.
    void addState(int id, std::unique_ptr<SpriteSheet> sheet,
        std::function<bool()> isActive, int priority);

    // Re-evaluates which registered state is active right now (the
    // lowest-priority state, among however many currently have a true
    // isActive(), wins), advances that one state's own animation clock by
    // dt, and resets every OTHER state's clock back to frame 0 (so
    // whichever one becomes active next always starts fresh instead of
    // resuming mid-cycle). Call exactly once per frame, before reading any
    // of the query methods below.
    void update(float dt);

    // Whether any registered state's isActive() currently holds. False
    // right after construction (before the first update()), and false if
    // no registered state's predicate is currently true - callers use this
    // to fall back to their own default appearance (e.g. a static sprite),
    // exactly as if this class weren't involved at all.
    bool hasActiveState() const { return m_activeIndex != npos; }

    // Whether at least one state has ever been registered via addState(),
    // regardless of whether one is active right now. Lets an owner
    // distinguish "I opted into this system at all" from "nothing of mine
    // happens to be active this frame" - the two need different fallback
    // behavior (see Monster::draw).
    bool hasAnyState() const { return !m_states.empty(); }

    // Applies the active state's current frame onto `sprite` (texture,
    // texture rect, origin - see SpriteSheet::applyCurrentFrame). No-op
    // while hasActiveState() is false.
    void applyCurrentFrame(sf::Sprite& sprite) const;

    // The active state's own base scale (see SpriteSheet::getBaseScale).
    // Meaningless (returns 1.f) while hasActiveState() is false.
    float getActiveBaseScale() const;

    // The active state's own id, exactly as passed to addState() - lets an
    // owner ask "is THIS specific one of my states showing right now"
    // without this class needing to know what the id represents. -1 while
    // hasActiveState() is false (never a valid id a caller would have
    // registered, by convention - ids are expected to start at 0).
    int getActiveStateId() const;

    // Whether the state registered under `id` has finished playing its
    // current animation (see SpriteSheet::isFinished - always false for a
    // looping sheet, meaningful only for a one-shot one). Returns true
    // (nothing to wait for) if no state was ever registered under `id` -
    // so a caller can ask this unconditionally, whether or not it bothered
    // to register that particular state at all.
    bool isStateFinished(int id) const;

private:
    struct Entry
    {
        int id;
        std::unique_ptr<SpriteSheet> sheet;
        std::function<bool()> isActive;
        int priority;
    };

    static constexpr std::size_t npos = static_cast<std::size_t>(-1);

    // Kept sorted by ascending priority at all times (see addState) - so
    // update() only ever needs a single front-to-back scan, first match
    // wins.
    std::vector<Entry> m_states;
    std::size_t m_activeIndex = npos;
};
