#pragma once

// A generic frame-index timeline: given a frame count and per-frame
// duration, tracks which frame is current over time. Knows nothing about
// textures or sprite sheets - see SpriteSheet for that layer.
class FrameAnimation
{
public:
    FrameAnimation(int frameCount, float frameDuration, bool looping = true);

    void update(float dt);
    void reset();
    int getCurrentFrameIndex() const;
    bool isFinished() const;

private:
    int m_frameCount;
    float m_frameDuration;
    float m_elapsed = 0.f;
    bool m_looping; // true: wraps forever; false: holds on the last frame
};
