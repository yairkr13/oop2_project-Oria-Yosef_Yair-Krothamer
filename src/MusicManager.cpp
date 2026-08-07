#include "MusicManager.h"
#include "AssetsManager.h"
#include <stdexcept>

namespace
{
    constexpr float FULL_VOLUME = 100.f;
    constexpr float MUTED_VOLUME = 0.f;

    const std::string MENU_TRACK_NAME = "menu_music";
    const std::string GAMEPLAY_TRACK_NAME = "gameplay_music";
}

const std::string& MusicManager::assetNameFor(MusicTrack track)
{
    switch (track)
    {
    case MusicTrack::Menu:     return MENU_TRACK_NAME;
    case MusicTrack::Gameplay: return GAMEPLAY_TRACK_NAME;
    default:
        throw std::logic_error("MusicTrack::None has no backing asset");
    }
}

void MusicManager::playTrack(MusicTrack track)
{
    if (track == m_currentTrack)
        return; // already showing the right track (or correctly silent)

    if (m_currentTrack != MusicTrack::None)
        AssetsManager::getInstance().getMusic(assetNameFor(m_currentTrack)).stop();

    m_currentTrack = track;
    if (track == MusicTrack::None)
        return;

    sf::Music& music = AssetsManager::getInstance().getMusic(assetNameFor(track));
    music.setLooping(true);
    music.setVolume(m_muted ? MUTED_VOLUME : FULL_VOLUME);
    music.play();
}

void MusicManager::toggleMute()
{
    m_muted = !m_muted;

    if (m_currentTrack != MusicTrack::None)
        AssetsManager::getInstance().getMusic(assetNameFor(m_currentTrack)).setVolume(m_muted ? MUTED_VOLUME : FULL_VOLUME);
}
