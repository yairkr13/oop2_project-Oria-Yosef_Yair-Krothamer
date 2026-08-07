#include "MusicManager.h"
#include "AssetsManager.h"

namespace
{
    constexpr float FULL_VOLUME = 100.f;
    constexpr float MUTED_VOLUME = 0.f;
}

void MusicManager::playMenuMusic()
{
    sf::Music& music = AssetsManager::getInstance().getMusic("menu_music");
    if (music.getStatus() == sf::Music::Status::Playing)
        return;

    music.setLooping(true);
    music.setVolume(m_muted ? MUTED_VOLUME : FULL_VOLUME);
    music.play();
}

void MusicManager::stopMenuMusic()
{
    sf::Music& music = AssetsManager::getInstance().getMusic("menu_music");
    if (music.getStatus() != sf::Music::Status::Stopped)
        music.stop();
}

void MusicManager::toggleMute()
{
    m_muted = !m_muted;
    AssetsManager::getInstance().getMusic("menu_music").setVolume(m_muted ? MUTED_VOLUME : FULL_VOLUME);
}
