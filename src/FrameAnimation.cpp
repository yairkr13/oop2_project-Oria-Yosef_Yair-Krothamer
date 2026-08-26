#include "FrameAnimation.h"
#include <algorithm>

FrameAnimation::FrameAnimation(int frameCount, float frameDuration, bool looping)
    : m_frameCount(std::max(frameCount, 1)), m_frameDuration(frameDuration), m_looping(looping)
{
}

void FrameAnimation::update(float dt)
{
    m_elapsed += dt;
}

void FrameAnimation::reset()
{
    m_elapsed = 0.f;
}

bool FrameAnimation::isFinished() const
{
    if (m_looping || m_frameDuration <= 0.f || m_frameCount <= 0) return false;
    return m_elapsed >= m_frameDuration * static_cast<float>(m_frameCount);
}

int FrameAnimation::getCurrentFrameIndex() const
{
    if (m_frameDuration <= 0.f || m_frameCount <= 0) return 0;

    int rawIndex = static_cast<int>(m_elapsed / m_frameDuration);
    // Looping wraps forever; non-looping clamps at the last frame instead
    // of wrapping back to the first once done, so it visibly holds there
    // rather than restarting - isFinished() above is what reports that
    // this happened.
    return m_looping ? (rawIndex % m_frameCount)
                      : std::min(rawIndex, m_frameCount - 1);
}
