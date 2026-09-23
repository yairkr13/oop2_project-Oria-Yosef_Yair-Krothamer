#include "SoundPlayer.h"
#include "AssetsManager.h"
#include <algorithm>

SoundPlayer& SoundPlayer::getInstance()
{
    static SoundPlayer instance;
    return instance;
}

// Fire-and-forget: each call gets its own sf::Sound, so overlapping plays
// don't cut each other off.
void SoundPlayer::play(const std::string& name)
{
    if (m_muted) return;
    // Prune finished sounds first, so the list never grows unbounded.
    m_activeSounds.erase(
        std::remove_if(m_activeSounds.begin(), m_activeSounds.end(),
            [](const sf::Sound& sound) { return sound.getStatus() == sf::Sound::Status::Stopped; }),
        m_activeSounds.end());

    const sf::SoundBuffer& buffer = AssetsManager::getInstance().getSoundBuffer(name);
    m_activeSounds.emplace_back(buffer);
    m_activeSounds.back().play();
}

void SoundPlayer::toggleMute()
{
    m_muted = !m_muted;
}

bool SoundPlayer::isMuted() const
{
    return m_muted;
}