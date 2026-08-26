#pragma once

// A generic, reusable frame-index timeline: given a fixed number of frames
// and a fixed duration per frame, tracks "which frame index is current"
// over time. Deliberately knows nothing about textures, sprite sheets,
// grids, rows/columns, or any particular kind of entity (monster or
// otherwise) - see SpriteSheet, which layers texture/grid knowledge on top
// of this to turn a frame index into an actual texture rect. On its own
// this class only ever answers "what frame, and are we done" - reusable
// for any animated entity's timeline, not just sprite sheets.
class FrameAnimation
{
public:
    // `looping` (default true): true wraps back to frame 0 forever after
    // the last frame - what a continuous animation (Idle/Walk/Attack)
    // wants. false instead holds on the last frame once reached and
    // reports it via isFinished() - what a one-shot animation (Death)
    // needs.
    FrameAnimation(int frameCount, float frameDuration, bool looping = true);

    // Advances the animation clock - call only while the animation should
    // actually be playing (e.g. only while the owning entity is in the
    // state this timeline represents).
    void update(float dt);

    // Restarts from the first frame - call whenever playback should begin
    // fresh (e.g. this animation's state just became inactive, ready to
    // start clean the next time it's active again).
    void reset();

    // The current frame index, always in [0, frameCount).
    int getCurrentFrameIndex() const;

    // True once a non-looping animation has played through its last frame
    // and is now holding on it. Always false for a looping animation - it
    // never "finishes", it just keeps wrapping.
    bool isFinished() const;

private:
    int m_frameCount;
    float m_frameDuration;
    float m_elapsed = 0.f;
    bool m_looping;
};
