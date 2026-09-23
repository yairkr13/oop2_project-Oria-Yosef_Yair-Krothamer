#include "SoundPlayer.h"
#include "AssetsManager.h"
#include <algorithm>

SoundPlayer& SoundPlayer::getInstance()
{
    static SoundPlayer instance;
    return instance;
}

void SoundPlayer::play(const std::string& name)
{
    if (m_muted) return;
    // מנקים קודם צלילים שכבר נגמרו - כדי שהרשימה לא תגדל בלי גבול לאורך משחק ארוך.
    m_activeSounds.erase(
        std::remove_if(m_activeSounds.begin(), m_activeSounds.end(),
            [](const sf::Sound& sound) { return sound.getStatus() == sf::Sound::Status::Stopped; }),
        m_activeSounds.end());

    const sf::SoundBuffer& buffer = AssetsManager::getInstance().getSoundBuffer(name);
	//create a new sf::Sound object and add it to the vector of active sounds
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